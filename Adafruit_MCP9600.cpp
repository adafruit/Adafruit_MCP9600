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
    @brief  Sets up the I2C connection and tests that the sensor was found.

    @param i2c_dev  Pointer to the Adafruit_I2CDevice instance to use.

    @return true if sensor was found, otherwise false.
*/
/**************************************************************************/
boolean Adafruit_MCP9600::begin(uint8_t i2c_addr, TwoWire *theWire) {
  i2c_dev = new Adafruit_I2CDevice(i2c_addr, theWire);
  
  /* Try to instantiate the I2C device. */
  if (!i2c_dev->begin()) {
    return false;
  }
  
  /* Check for MCP9600 device ID and revision register (0x20), high byte should be 0x40. */
  Adafruit_I2CRegister id_reg = Adafruit_I2CRegister(i2c_dev, MCP9600_DEVICEID, 2, MSBFIRST);

  if ((id_reg.read() & 0xFF00) != 0x4000) {
    return false;
  }

  // define the config register
  _config_reg = new Adafruit_I2CRegister(i2c_dev, MCP9600_DEVICECONFIG);
  _config_reg->write(0x80);
  enable(true);

  return true;
}


float Adafruit_MCP9600::readThermocouple(void) {
  if (!enabled()) {
    return NAN;
  }

  // define the register
  Adafruit_I2CRegister therm_reg = Adafruit_I2CRegister(i2c_dev, MCP9600_HOTJUNCTION, 2, MSBFIRST);

  // read a signed 16 bit value
  int16_t therm = therm_reg.read();

  // convert to floating and shift to celsius
  float temp = therm;
  temp *=  0.0625;  // 0.0625*C per LSB!
  return temp;
}

float Adafruit_MCP9600::readAmbient(void) {
  if (!enabled()) {
    return NAN;
  }

  // define the register
  Adafruit_I2CRegister cold_reg = 
    Adafruit_I2CRegister(i2c_dev, MCP9600_COLDJUNCTION, 2, MSBFIRST);

  // read a signed 16 bit value
  int16_t cold = cold_reg.read();

  // convert to floating and shift to celsius
  float temp = cold;
  temp *=  0.0625;  // 0.0625*C per LSB!
  return temp;
}

void Adafruit_MCP9600::enable(bool flag) {
  // define the status bits
  Adafruit_I2CRegisterBits status = 
    Adafruit_I2CRegisterBits(_config_reg, 2, 0);  // # bits, bit_shift

  if (!flag) { // sleep mode
    status.write(0x01);
  } else {
    status.write(0x00);
  }
}

bool Adafruit_MCP9600::enabled(void) {
  // define the status bits
  Adafruit_I2CRegisterBits status = 
    Adafruit_I2CRegisterBits(_config_reg, 2, 0);  // # bits, bit_shift

  return !status.read();
}

void Adafruit_MCP9600::setADCresolution(MCP9600_ADCResolution resolution) {
  // define the resolution bits
  Adafruit_I2CRegisterBits res = 
    Adafruit_I2CRegisterBits(_config_reg, 2, 5);  // # bits, bit_shift

  res.write(resolution);
}

MCP9600_ADCResolution Adafruit_MCP9600::getADCresolution(void) {
  // define the resolution bits
  Adafruit_I2CRegisterBits res = 
    Adafruit_I2CRegisterBits(_config_reg, 2, 5);  // # bits, bit_shift

  return (MCP9600_ADCResolution)res.read();
}

int32_t Adafruit_MCP9600::readADC(void) {
  // define the register
  Adafruit_I2CRegister adc = 
    Adafruit_I2CRegister(i2c_dev, MCP9600_RAWDATAADC, 3, MSBFIRST);
  uint32_t reading = adc.read();
  // extend 24 bits to 32
  if (reading & 0x800000) {
    reading |= 0xFF000000;
  }
  return reading;
}


MCP9600_ThemocoupleType Adafruit_MCP9600::getThermocoupleType(void) {
  // define the register
  Adafruit_I2CRegister sensorconfig = 
    Adafruit_I2CRegister(i2c_dev, MCP9600_SENSORCONFIG, 1, MSBFIRST);
  Adafruit_I2CRegisterBits type = 
    Adafruit_I2CRegisterBits(&sensorconfig, 3, 4);  // # bits, bit_shift

  return (MCP9600_ThemocoupleType)type.read();
}

void Adafruit_MCP9600::setThermocoupleType(MCP9600_ThemocoupleType thermotype) {
  // define the register
  Adafruit_I2CRegister sensorconfig = 
    Adafruit_I2CRegister(i2c_dev, MCP9600_SENSORCONFIG, 1, MSBFIRST);
  Adafruit_I2CRegisterBits type = 
    Adafruit_I2CRegisterBits(&sensorconfig, 3, 4);  // # bits, bit_shift

  type.write(thermotype);
}

uint8_t Adafruit_MCP9600::getFilterCoefficient(void) {
  // define the register
  Adafruit_I2CRegister sensorconfig = 
    Adafruit_I2CRegister(i2c_dev, MCP9600_SENSORCONFIG, 1, MSBFIRST);
  Adafruit_I2CRegisterBits filter = 
    Adafruit_I2CRegisterBits(&sensorconfig, 3, 0);  // # bits, bit_shift

  return filter.read();
}

void Adafruit_MCP9600::setFilterCoefficient(uint8_t filtercount) {
  // define the register
  Adafruit_I2CRegister sensorconfig = 
    Adafruit_I2CRegister(i2c_dev, MCP9600_SENSORCONFIG, 1, MSBFIRST);
  Adafruit_I2CRegisterBits filter = 
    Adafruit_I2CRegisterBits(&sensorconfig, 3, 0);  // # bits, bit_shift

  filter.write(filtercount);
}


float Adafruit_MCP9600::getAlertTemperature(uint8_t alert) {
  if ((alert < 1) || (alert > 4)) 
    return NAN; // invalid
  // define the register
  Adafruit_I2CRegister alerttemp = 
    Adafruit_I2CRegister(i2c_dev, MCP9600_ALERTLIMIT_1+alert-1, 2, MSBFIRST);

  // read a signed 16 bit value
  int16_t therm = alerttemp.read();

  // convert to floating and shift to celsius
  float temp = therm;
  temp *=  0.0625;  // 0.0625*C per LSB!
  return temp;
}

void Adafruit_MCP9600::setAlertTemperature(uint8_t alert, float temp) {
  if ((alert < 1) || (alert > 4)) 
    return; // invalid

  // define the register
  Adafruit_I2CRegister alerttemp = 
    Adafruit_I2CRegister(i2c_dev, MCP9600_ALERTLIMIT_1+alert-1, 2, MSBFIRST);

  int16_t therm = temp / 0.0625;  // 0.0625*C per LSB!
  alerttemp.write(therm);
}

void Adafruit_MCP9600::configureAlert(uint8_t alert, bool enabled, bool rising, 
				      bool alertColdJunction,
				      bool activeHigh, 
				      bool interruptMode) {
  if ((alert < 1) || (alert > 4)) 
    return; // invalid
  // define the register
  Adafruit_I2CRegister alertconfig = 
    Adafruit_I2CRegister(i2c_dev, MCP9600_ALERTCONFIG_1+alert-1, 1, MSBFIRST);

  uint8_t c = 0;

  if (enabled) {
    c |= 0x1;
  }
  if (interruptMode) {
    c |= 0x2;
  }
  if (activeHigh) {
    c |= 0x4;
  }
  if (rising) {
    c |= 0x8;
  }
  if (alertColdJunction) {
    c |= 0x10;
  }
  alertconfig.write(c);
}
