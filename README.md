<<<<<<< HEAD
AS5048A-Arduino
===============

A simple SPI library to interface with Austria Microsystem's AS5048A angle sensor with an Arduino.

The sensor should be connected to the hardware SPI pins (MISO, MOSI, SCK). The CS pin can be connected to any GPIO pin but should be passed to the constructor.

The angle sensor is described in more detail [here](zoetrope.io/AS5048)
=======
# AS5048A_DUE
This is an API I augmented from the wrok of ZoetropeLabs. I modified it to work with the Arduino DUE microcontroller SerialUSB protocol which takes advantage of the Native USB Host on the ARM M3 board. I also updated the respective timing clocks to fit within the 4-5 MHz range prescribed by the AS5048a datasheet.  
>>>>>>> a95186082b20b587d32bf15d627c375e49e12e11
