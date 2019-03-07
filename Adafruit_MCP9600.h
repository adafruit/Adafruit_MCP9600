/**************************************************************************/
/**
  @file     Adafruit_MCP9600.h

  Author: K. Townsend (Adafruit Industries)
  License: BSD (see license.txt)

  This is a library for the Aadafruit MCP9600 I2C Thermocouple breakout board
  ----> http://www.adafruit.com/products/

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  v1.0  - First release
*/
/**************************************************************************/
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_I2CRegister.h>

/** Default MCP9600 I2C address. */
#define MCP9600_I2CADDR_DEFAULT 0xFF

/**************************************************************************/
/*!
    @brief  MCP9600 driver.
*/
/**************************************************************************/
class Adafruit_MCP9600 {
    public:
        Adafruit_MCP9600();
        boolean begin(Adafruit_I2CDevice *i2c_dev);

    private:
        Adafruit_I2CDevice *_i2c_dev;
};
