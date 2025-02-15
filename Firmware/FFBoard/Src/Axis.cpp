/*
 * Axis.cpp
 *
 *  Created on: 31.01.2020
 *      Author: Yannick
 */

#include "Axis.h"
#include "voltagesense.h"


//////////////////////////////////////////////
/*
 * Sources for class choosers are defined in MotorDriver and Encoder
 */


// TODO class type for parser? (Simhub for example)
//////////////////////////////////////////////

Axis::Axis(char axis,volatile Control_t* control) : NormalizedAxis(axis), drv_chooser(MotorDriver::all_drivers),enc_chooser{Encoder::all_encoders}
{
	// Create HID FFB handler. Will receive all usb messages directly
//	this->axis = axis;
	this->control = control;
	if (axis == 'X')
	{
		this->flashAddrs = AxisFlashAddrs({ADR_AXIS1_CONFIG});
	}
	else if (axis == 'Y')
	{
		this->flashAddrs = AxisFlashAddrs({ADR_AXIS2_CONFIG});
	}
	else if (axis == 'Z')
	{
		this->flashAddrs = AxisFlashAddrs({ADR_AXIS3_CONFIG});
	}

	restoreFlash(); // Load parameters
}

Axis::~Axis()
{

}

/*
 * Read parameters from flash and restore settings
 */
void Axis::restoreFlash(){
	NormalizedAxis::restoreFlash();
	// read all constants
	uint16_t confint;
	if (Flash_Read(flashAddrs.config, &confint)){
		this->conf = Axis::decodeConfFromInt(confint);
	}else{
		pulseErrLed();
	}

	setDrvType(this->conf.drvtype);
	setEncType(this->conf.enctype);
}
// Saves parameters to flash. Inherited via Normalized Axis. Must call parent too
void Axis::saveFlash(){
	NormalizedAxis::saveFlash();
	Flash_Write(flashAddrs.config, Axis::encodeConfToInt(this->conf));
}


uint8_t Axis::getDrvType(){
	return (uint8_t)this->conf.drvtype;
}

uint8_t Axis::getEncType(){
	if(drv->hasIntegratedEncoder()){
		return 255;
	}
	return (uint8_t)this->conf.enctype;
}


void Axis::setPos(uint16_t val)
{
	if(this->drv != nullptr){
		drv->getEncoder()->setPos(val);
	}
}

/*
 * Called from FFBWheel->Update() via AxesManager->Update()
 */
void Axis::prepareForUpdate(){
	if (drv == nullptr){
		pulseErrLed();
		return;
	}

	if (!drv->motorReady()) return;

	// Scale encoder value to set rotation range
	// Update a change of range only when new range is within valid range
	if (nextDegreesOfRotation != degreesOfRotation && abs(getEncValue(drv->getEncoder(), nextDegreesOfRotation)) < 0x7fff){
		degreesOfRotation = nextDegreesOfRotation;
	}
	// scaledEnc now gets inverted if necessary in updateMetrics
	int32_t scaledEnc = getEncValue(drv->getEncoder(), degreesOfRotation);

	if (abs(scaledEnc) > 0xffff){
		// We are way off. Shut down
		drv->stopMotor();
		pulseErrLed();
	}

	this->updateMetrics(scaledEnc);

}

void Axis::updateDriveTorque(){
	// totalTorque = effectTorque + endstopTorque
	int32_t totalTorque;
	bool torqueChanged = updateTorque(&totalTorque);
	if (torqueChanged && drv->motorReady()){
		// Send to motor driver
		drv->turn(totalTorque);
	}
}

void Axis::setPower(uint16_t power)
{
	NormalizedAxis::setPower(power);
	// Update hardware limits for TMC for safety
	if (this->conf.drvtype == TMC4671::info.id)
	{
		TMC4671 *drv = static_cast<TMC4671 *>(this->drv.get());
		//tmclimits.pid_uq_ud = power;
		//tmclimits.pid_torque_flux = power;
		drv->setTorqueLimit(power);
	}
}


// create and setup a motor driver
void Axis::setDrvType(uint8_t drvtype)
{
	if (!drv_chooser.isValidClassId(drvtype))
	{
		return;
	}
	this->drv.reset(nullptr);
	MotorDriver* drv = drv_chooser.Create((uint16_t)drvtype);
	if (drv == nullptr)
	{
		return;
	}
	this->drv = std::unique_ptr<MotorDriver>(drv);
	this->conf.drvtype = drvtype;

	if (dynamic_cast<TMC4671 *>(drv))
	{
		setupTMC4671();
	}

	if (!tud_connected())
	{
		control->usb_disabled = false;
		this->usbSuspend();
	}
	else
	{
		drv->startMotor();
	}
}

// Special tmc setup methods
void Axis::setupTMC4671()
{
	this->setupTMC4671ForAxis(axis);
}

void Axis::setupTMC4671ForAxis(char axis)
{
	TMC4671 *drv = static_cast<TMC4671 *>(this->drv.get());
	drv->setAxis(axis);
	drv->restoreFlash();
	tmclimits.pid_torque_flux = getPower();
	drv->setLimits(tmclimits);
	//drv->setBiquadFlux(fluxbq);
	
	if (tmcFeedForward){
		drv->setupFeedForwardTorque(torqueFFgain, torqueFFconst);
		drv->setupFeedForwardVelocity(velocityFFgain, velocityFFconst);
		drv->setFFMode(FFMode::torque);
	}
	// Enable driver

	drv->setMotionMode(MotionMode::torque);
	drv->Start(); // Start thread
}

void Axis::setEncType(uint8_t enctype)
{

	if (enc_chooser.isValidClassId(enctype) && !drv->hasIntegratedEncoder())
	{

		this->conf.enctype = (enctype);
		this->enc = std::shared_ptr<Encoder>(enc_chooser.Create(enctype)); // Make new encoder
		if(drv && !drv->hasIntegratedEncoder())
			this->drv->setEncoder(this->enc);
	}


	int32_t scaledEnc = getEncValue(this->drv->getEncoder(), degreesOfRotation);
	this->resetMetrics(scaledEnc);
}

/*
 * Returns a scaled encoder value between -0x7fff and 0x7fff with a range of degrees
 */
//TODO JL - use a precalc scaler to make this more efficient
int32_t Axis::getEncValue(Encoder *enc, uint16_t degrees){
	if (enc == nullptr || degrees == 0){
		return 0x7fff; // Return center if no encoder present
	}
	float angle = 360.0 * enc->getPos_f();
	int32_t val = (0xffff / (float)degrees) * angle;
	if (isInverted()){
		val= -val;
	}
	return val;
}

void Axis::emergencyStop(){
	drv->stopMotor();
	control->emergency = true;
}

void Axis::usbSuspend(){
	if (drv != nullptr){
		drv->turn(0);
		drv->stopMotor();
	}
}
void Axis::usbResume(){
	if (drv != nullptr){
		drv->startMotor();
	}
}

void Axis::processHidCommand(HID_Custom_Data_t *data){
	uint8_t axis = (data->cmd >> 6) & 0x3;
	if(axis != this->axis){
		return;
	}


	switch (data->cmd&0x3F){
		case HID_CMD_FFB_STRENGTH:
			if (data->type == HidCmdType::write){
				setPower(data->data);
			}
			else if (data->type == HidCmdType::request){
				data->data = getPower();
			}
		break;

		case HID_CMD_FFB_ZERO:
			if (data->type == HidCmdType::write){
				this->setPos(0);
			}
		break;

		default:

		break;
	}
	return;
}


// { drvtype, power, zeroenc, enctype, cpr, pos, degrees
//, esgain, fxratio, idlespring, friction, invert,  };

ParseStatus Axis::command(ParsedCommand *cmd, std::string *reply)
{
	if ((cmd->prefix & 0xDF) != axis){
		return ParseStatus::NOT_FOUND;
	}

	// Check the super class command handler first
	ParseStatus flag = NormalizedAxis::command(cmd, reply);
	if ( flag == ParseStatus::OK) {
		return flag;
	}

	flag = ParseStatus::OK;

	if (cmd->cmd == "drvtype")
	{
		if (cmd->type == CMDtype::get)
		{
			*reply += std::to_string(this->getDrvType());
		}
		else if (cmd->type == CMDtype::set && this->drv_chooser.isValidClassId(cmd->val))
		{
			setDrvType((cmd->val));
		}
		else
		{
			*reply += drv_chooser.printAvailableClasses(this->conf.drvtype);
		}
	}
	else if (cmd->cmd == "zeroenc")
	{
		if (cmd->type == CMDtype::get)
		{
			this->setPos(0);
			*reply += "Zeroed";
		}
	}
	else if (cmd->cmd == "enctype")
	{
		if (cmd->type == CMDtype::get)
		{
			*reply += std::to_string(this->getEncType());
		}
		else if (cmd->type == CMDtype::set)
		{
			setEncType(cmd->val);
		}
		else
		{
			if(this->drv->hasIntegratedEncoder()){
				*reply += "255:0:"+std::string(this->drv->getInfo().name); // TODO dynamic?
			}else{
				*reply += enc_chooser.printAvailableClasses(this->conf.enctype);
			}

		}
	}
	else if (cmd->cmd == "pos")
	{
		if (cmd->type == CMDtype::get)
		{
			*reply += std::to_string(this->drv->getEncoder()->getPos());
		}
		else if (cmd->type == CMDtype::set && this->drv->getEncoder() != nullptr)
		{
			this->drv->getEncoder()->setPos(cmd->val);
		}
		else
		{
			flag = ParseStatus::ERR;
			*reply += "Err. Setup encoder first";
		}
	}
	else{
		flag = ParseStatus::NOT_FOUND;
	}

	return flag;
};


/*
 * Helper functions for encoding and decoding flash variables
 */
AxisConfig Axis::decodeConfFromInt(uint16_t val)
{
	// 0-6 enc, 7-12 Mot
	AxisConfig conf;
	conf.enctype = ((val)&0x3f);
	conf.drvtype = ((val >> 6) & 0x3f);
	return conf;
}
uint16_t Axis::encodeConfToInt(AxisConfig conf)
{
	uint16_t val = (uint8_t)conf.enctype & 0x3f;
	val |= ((uint8_t)conf.drvtype & 0x3f) << 6;
	return val;
}
