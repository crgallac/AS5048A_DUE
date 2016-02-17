#include <SPI.h>
#include <AS5048A.h>

bool STATE=HIGH; 
AS5048A angleSensor(10);

void setup()
{
	SerialUSB.begin(9600);
	angleSensor.init();
while (!Serial) ;
  pinMode(13, OUTPUT);
}

void loop()
{
	delay(1);
  STATE=!STATE; 
digitalWrite(13, !STATE);  
  SerialUSB.println(); 
	word val = angleSensor.getRawRotation();
	SerialUSB.println("Got rotation of: 0x");
	SerialUSB.println(val, HEX);
	SerialUSB.println("State:");
  SerialUSB.println(STATE); 
	angleSensor.printState();
	SerialUSB.println("Errors: ");
	SerialUSB.println(angleSensor.getErrors());
}
