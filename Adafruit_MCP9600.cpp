/**************************************************************************/
/*!
  @file     Adafruit_MCP9600.cpp

  @mainpage Adafruit MCP9600 I2C Thermocouple ADC driver

  @section intro Introduction

  This is a library for the Adafruit MCP9600 breakout board
  ----> http://www.adafruit.com/products/

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  @section author Author

  K. Townsend (Adafruit Industries)

  @section license License

  BSD (see license.txt)
*/
/**************************************************************************/
#include "Adafruit_MCP9600.h"

/**************************************************************************/
/*!
    @brief  Instantiates a new MCP9600 class
*/
/**************************************************************************/
Adafruit_MCP9600::Adafruit_MCP9600() {

}

/**************************************************************************/
/*!
    @brief  Setups the I2C connection and tests that the sensor was found. If so, configures for 200mA IR current and 390.625 KHz.
    @param addr Optional I2C address (however, all chips have the same address!)
    @param theWire Optional Wire object if your board has more than one I2C interface
    @return true if sensor was found, false if not
*/
/**************************************************************************/
boolean Adafruit_MCP9600::begin(Adafruit_I2CDevice *i2c_dev) {
    /* Store a reference to the I2C device instance. */
    _i2c_dev = i2c_dev;

    /* Try to instantiate the I2C device. */
    if (!_i2c_dev->begin()) {
      return false;
    }

    /* Check for MCP9600 device ID. */
    Adafruit_I2CRegister reg = Adafruit_I2CRegister(_i2c_dev, 0x00);
    uint8_t id;
    reg.read(&id);
    if (id != 0b11100101) {
        return false;
    }

    return true;
}
