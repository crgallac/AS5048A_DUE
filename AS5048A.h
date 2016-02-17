#include <stdint.h>
#include <SPI.h>
#ifndef as5048A_h
#define as5048A_h
#define LIBRARY_VERSION 1.0.0

#define AS5048A_SPI_NOP 0x0000 // No Operation Dummy information
#define AS5048A_CLEAR_ERROR_FLAG 0x0001
#define AS5048A_PROGRAMMING_CONTROL 0x0003
#define AS5048A_OTP_REGISTER_ZERO_POS_HIGH 0x0016// OTP-> one time programmable  // Bits 6-13
#define AS5048A_OTP_REGISTER_ZERO_POS_LOW 0x0017 // bits 0-5
#define AS5048A_DIAG_AGC 0x3FFD // Diagnostics and automatic gain control (AGC) - use for magnetic field strength
#define AS5048A_MAGNITUDE 0x3FFE // Magnitude information after ATAN calculation
#define AS5048A_ANGLE 0x3FFF // Magnitude information after ATAN calculation and zero position adder
#define SPI_CLOCK_4_2MHz 0x0014 //sets SPI clock to 4 MHz

class AS5048A{

	bool errorFlag;
	uint8_t _cs;
	uint8_t cs;
	uint8_t dout;
	uint8_t din;
	uint8_t clk;
	uint16_t position;
	uint16_t transaction(uint16_t data);
	
	public:

	/**
	 *	Constructor
	 */
	AS5048A(uint8_t arg_cs);

	/**
	*	Destructor
	*/
	~AS5048A();

	/**
	 * Initialiser
	 * Sets up the SPI interface
	 */
	void init();

	/**
	 * Closes the SPI connection
	 */
	void close();

	/*
	 * Read a register from the sensor
	 * Takes the address of the register as a 16 bit uint16_t
	 * Returns the value of the register
	 */
	uint16_t read(uint16_t registerAddress);

	/*
	 * Write to a register
	 * Takes the 16-bit  address of the target register and the 16 bit uint16_t of data
	 * to be written to that register
	 * Returns the value of the register after the write has been performed. This
	 * is read back from the sensor to ensure a sucessful write.
	 */
	uint16_t write(uint16_t registerAddress, uint16_t data);

	/**
	 * Get the rotation of the sensor relative to the zero position.
	 *
	 * @return {int} between -2^13 and 2^13
	 */
	int getRotation();

	/**
	 * Returns the raw angle directly from the sensor
	 */
	uint16_t getRawRotation();


	/**
	 * returns the value of the state register
	 * @return 16 bit uint16_t containing flags
	 */
	uint16_t getState();

	/**
	 * Print the diagnostic register of the sensor
	 */
	void printState();

	/**
	 * Returns the value used for Automatic Gain Control (Part of diagnostic
	 * register)
	 */
	uint8_t getGain();

	/*
	 * Get and clear the error register by reading it
	 */
	uint16_t getErrors();

	/*
	 * Set the zero position
	 */
	void setZeroPosition(uint16_t arg_position);

	/*
	 * Returns the current zero position
	 */
	uint16_t getZeroPosition();

	/*
	 * Check if an error has been encountered.
	 */
	bool error();

	private:

	uint8_t spiCalcEvenParity(uint16_t);
};
#endif
