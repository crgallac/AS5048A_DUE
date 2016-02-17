#if ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include <AS5048A.h>
//#include <SPI.h>

//#define AS5048A_DEBUG

/**
 * Constructor
 */
AS5048A::AS5048A(uint8_t arg_cs){
	_cs = arg_cs;
	errorFlag = false;
	position = 0;

	//setup pins
	pinMode(_cs,OUTPUT);

}


/**
 * Initialiser
 * Sets up the SPI interface
 */
void AS5048A::init()
{
	SPI.setDataMode(SPI_MODE1);
	SPI.setClockDivider(SPI_CLOCK_4MHz);
	SPI.setBitOrder(MSBFIRST);
	SPI.begin(_cs);
}

/**
 * Closes the SPI connection
 */
void AS5048A::close()
{
	SPI.end();
}

/**
 * Utility function used to calculate even parity of word
 */
uint8_t AS5048A::spiCalcEvenParity(uint16_t value)
{
	uint8_t cnt = 0;
	uint8_t i;

	for (i = 0; i < 16; i++)
	{
		if (value & 0x1)
		{
			cnt++;
		}
		value >>= 1;
	}
	return cnt & 0x1;
}



/**
 * Get the rotation of the sensor relative to the zero position.
 *
 * @return {int} between -2^13 and 2^13
 */
int AS5048A::getRotation(){
	uint16_t data;
	int rotation;

	data = AS5048A::getRawRotation();
	rotation = (int)data - (int)position;
	if(rotation > 8191) rotation = -((0x3FFF)-rotation); //more than -180
	//if(rotation < -0x1FFF) rotation = rotation+0x3FFF;

	return rotation;
}

/**
 * Returns the raw angle directly from the sensor
 */
uint16_t AS5048A::getRawRotation(){
	return AS5048A::read(AS5048A_ANGLE);
}

/**
 * returns the value of the state register
 * @return 16 bit word containing flags
 */
uint16_t AS5048A::getState(){
	return AS5048A::read(AS5048A_DIAG_AGC);
}

/**
 * Print the diagnostic register of the sensor
 */
void AS5048A::printState(){
	uint16_t data;

	data = AS5048A::getState();
	if(AS5048A::error()){
		SerialUSB.print("Error bit was set!");
	}
	SerialUSB.println(data, BIN);
}

/**
 * Returns the value used for Automatic Gain Control (Part of diagnostic
 * register)
 */
uint8_t AS5048A::getGain(){
	uint16_t data = AS5048A::getState();
	return (uint8_t) data & 0xFF;
}

/*
 * Get and clear the error register by reading it
 */
uint16_t AS5048A::getErrors(){
	return AS5048A::read(AS5048A_CLEAR_ERROR_FLAG);
}

/*
 * Set the zero position
 */
void AS5048A::setZeroPosition(uint16_t arg_position){
	position = arg_position % 0x3FFF;
}

/*
 * Returns the current zero position
 */
uint16_t AS5048A::getZeroPosition(){
	return position;
}

/*
 * Check if an error has been encountered.
 */
bool AS5048A::error(){
	return errorFlag;
}

/*
 * Read a register from the sensor
 * Takes the address of the register as a 16 bit uint16_t
 * Returns the value of the register
 */
uint16_t AS5048A::read(uint16_t registerAddress){
	uint16_t command = 0b0100000000000000; // PAR=0 R/W=R
	command = command | registerAddress;

	//Add a parity bit on the the MSB
	command |= ((uint16_t)spiCalcEvenParity(command)<<15);

	//Split the command into two uint8_ts
	uint8_t right_byte = command & 0xFF;
	uint8_t left_byte = ( command >> 8 ) & 0xFF;

#ifdef AS5048A_DEBUG
	SerialUSB.print("Read (0x");
	SerialUSB.print(registerAddress, HEX);
	SerialUSB.print(") with command: 0b");
	SerialUSB.println(command, BIN);
#endif

	//Send the command
	digitalWrite(_cs, LOW);
	SPI.transfer(left_byte);
	SPI.transfer(right_byte);
	digitalWrite(_cs,HIGH);

	//Now read the response
	digitalWrite(_cs, LOW);
	left_byte = SPI.transfer(0x00);
	right_byte = SPI.transfer(0x00);
	digitalWrite(_cs, HIGH);

#ifdef AS5048A_DEBUG
	SerialUSB.print("Read returned: ");
	SerialUSB.print(left_byte, BIN);
	SerialUSB.print(" ");
	SerialUSB.println(right_byte, BIN);
#endif

	//Check if the error bit is set
	if (left_byte & 0x40) {
		SerialUSB.println("Setting Error bit");
		errorFlag = true;
	}
	else {
		errorFlag = false;
	}

	//Return the data, stripping the parity and error bits
	return (( ( left_byte & 0xFF ) << 8 ) | ( right_byte & 0xFF )) & ~0xC000;
}


/*
 * Write to a register
 * Takes the 16-bit  address of the target register and the 16 bit uint16_t of data
 * to be written to that register
 * Returns the value of the register after the write has been performed. This
 * is read back from the sensor to ensure a sucessful write.
 */
uint16_t AS5048A::write(uint16_t registerAddress, uint16_t data) {

	uint16_t command = 0b0000000000000000; // PAR=0 R/W=W
	command |= registerAddress;

	//Add a parity bit on the the MSB
	command |= ((uint16_t)spiCalcEvenParity(command)<<15);

	//Split the command into two uint8_ts
	uint8_t right_byte = command & 0xFF;
	uint8_t left_byte = ( command >> 8 ) & 0xFF;

#ifdef AS5048A_DEBUG
	SerialUSB.print("Write (0x");
	SerialUSB.print(registerAddress, HEX);
	SerialUSB.print(") with command: 0b");
	SerialUSB.println(command, BIN);
#endif

	//Start the write command with the target address
	digitalWrite(_cs, LOW);
	SPI.transfer(left_byte);
	SPI.transfer(right_byte);
	digitalWrite(_cs,HIGH);

	uint16_t dataToSend = 0b0000000000000000;
	dataToSend |= data;

	//Craft another packet including the data and parity
	dataToSend |= ((uint16_t)spiCalcEvenParity(dataToSend)<<15);
	right_byte = dataToSend & 0xFF;
	left_byte = ( dataToSend >> 8 ) & 0xFF;

#ifdef AS5048A_DEBUG
	SerialUSB.print("Sending data to write: ");
	SerialUSB.println(dataToSend, BIN);
#endif

	//Now send the data packet
	digitalWrite(_cs,LOW);
	SPI.transfer(left_byte);
	SPI.transfer(right_byte);
	digitalWrite(_cs,HIGH);

	//Send a NOP to get the new data in the register
	digitalWrite(_cs, LOW);
	left_byte =-SPI.transfer(0x00);
	right_byte = SPI.transfer(0x00);
	digitalWrite(_cs, HIGH);

	//Return the data, stripping the parity and error bits
	return (( ( left_byte & 0xFF ) << 8 ) | ( right_byte & 0xFF )) & ~0xC000;
}
