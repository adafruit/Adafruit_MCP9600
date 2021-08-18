/**************************************************************************/
/*!
  @file     Adafruit_MCP9600.cpp

  @mainpage Adafruit MCP9600 I2C Thermocouple ADC driver

  @section intro Introduction

  This is a library for the Adafruit MCP9600 breakout board
  ----> https://www.adafruit.com/product/4101

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
Adafruit_MCP9600::Adafruit_MCP9600() { _device_id = 0x40; }

/**************************************************************************/
/*!
    @brief  Sets up the I2C connection and tests that the sensor was found.
    @param i2c_addr The I2C address of the target device, default is 0x67
    @param theWire Pointer to an I2C device we'll use to communicate
    default is Wire
    @return true if sensor was found, otherwise false.
*/
/**************************************************************************/
boolean Adafruit_MCP9600::begin(uint8_t i2c_addr, TwoWire *theWire) {
  i2c_dev = new Adafruit_I2CDevice(i2c_addr, theWire);

  /* Try to instantiate the I2C device. */
  if (!i2c_dev->begin(false)) { // *dont scan!*
    return false;
  }

  /* Check for MCP9600 device ID and revision register (0x20), high byte should
   * be 0x40. */
  Adafruit_I2CRegister id_reg =
      Adafruit_I2CRegister(i2c_dev, MCP9600_DEVICEID, 2, MSBFIRST);

  if ((id_reg.read() >> 8) != _device_id) {
    return false;
  }

  // define the config register
  _config_reg = new Adafruit_I2CRegister(i2c_dev, MCP9600_DEVICECONFIG);
  _config_reg->write(0x80);
  enable(true);

  return true;
}

/**************************************************************************/
/*!
    @brief  Read temperature at the end of the thermocouple
    @return Floating point temperature in Centigrade
*/
/**************************************************************************/
float Adafruit_MCP9600::readThermocouple(void) {
  if (!enabled()) {
    return NAN;
  }

  // define the register
  Adafruit_I2CRegister therm_reg =
      Adafruit_I2CRegister(i2c_dev, MCP9600_HOTJUNCTION, 2, MSBFIRST);

  // read a signed 16 bit value
  int16_t therm = therm_reg.read();

  // convert to floating and shift to celsius
  float temp = therm;
  temp *= 0.0625; // 0.0625*C per LSB!
  return temp;
}

/**************************************************************************/
/*!
    @brief  Read temperature at the MCP9600 chip body
    @return Floating point temperature in Centigrade
*/
/**************************************************************************/
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
  temp *= 0.0625; // 0.0625*C per LSB!
  return temp;
}

/**************************************************************************/
/*!
    @brief  Whether to have the sensor enabled and working or in sleep mode
    @param  flag True to be in awake mode, False for sleep mode
*/
/**************************************************************************/
void Adafruit_MCP9600::enable(bool flag) {
  // define the status bits
  Adafruit_I2CRegisterBits status =
      Adafruit_I2CRegisterBits(_config_reg, 2, 0); // # bits, bit_shift

  if (!flag) { // sleep mode
    status.write(0x01);
  } else {
    status.write(0x00);
  }
}

/**************************************************************************/
/*!
    @brief  Whether the sensor is enabled and working or in sleep mode
    @returns True if in awake mode, False if in sleep mode
*/
/**************************************************************************/
bool Adafruit_MCP9600::enabled(void) {
  // define the status bits
  Adafruit_I2CRegisterBits status =
      Adafruit_I2CRegisterBits(_config_reg, 2, 0); // # bits, bit_shift

  return !status.read();
}

/**************************************************************************/
/*!
    @brief  The desired ADC resolution setter
    @param  resolution Can be MCP9600_ADCRESOLUTION_18,
    MCP9600_ADCRESOLUTION_16, MCP9600_ADCRESOLUTION_14,
    or MCP9600_ADCRESOLUTION_12.
*/
/**************************************************************************/
void Adafruit_MCP9600::setADCresolution(MCP9600_ADCResolution resolution) {
  // define the resolution bits
  Adafruit_I2CRegisterBits res =
      Adafruit_I2CRegisterBits(_config_reg, 2, 5); // # bits, bit_shift

  res.write(resolution);
}

/**************************************************************************/
/*!
    @brief  The desired ADC resolution getter
    @returns The reslution: MCP9600_ADCRESOLUTION_18,
    MCP9600_ADCRESOLUTION_16, MCP9600_ADCRESOLUTION_14,
    or MCP9600_ADCRESOLUTION_12.
*/
/**************************************************************************/
MCP9600_ADCResolution Adafruit_MCP9600::getADCresolution(void) {
  // define the resolution bits
  Adafruit_I2CRegisterBits res =
      Adafruit_I2CRegisterBits(_config_reg, 2, 5); // # bits, bit_shift

  return (MCP9600_ADCResolution)res.read();
}

/**************************************************************************/
/*!
    @brief  Read the raw ADC voltage, say for self calculating a temperature
    @returns The 32-bit signed value from the ADC DATA register
*/
/**************************************************************************/
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

/**************************************************************************/
/*!
    @brief  The desired thermocouple type getter
    @returns The selected type: MCP9600_TYPE_K, MCP9600_TYPE_J,
    MCP9600_TYPE_T, MCP9600_TYPE_N, MCP9600_TYPE_S, MCP9600_TYPE_E,
    MCP9600_TYPE_B or MCP9600_TYPE_R
*/
/**************************************************************************/
MCP9600_ThemocoupleType Adafruit_MCP9600::getThermocoupleType(void) {
  // define the register
  Adafruit_I2CRegister sensorconfig =
      Adafruit_I2CRegister(i2c_dev, MCP9600_SENSORCONFIG, 1, MSBFIRST);
  Adafruit_I2CRegisterBits type =
      Adafruit_I2CRegisterBits(&sensorconfig, 3, 4); // # bits, bit_shift

  return (MCP9600_ThemocoupleType)type.read();
}

/**************************************************************************/
/*!
    @brief  The desired thermocouple type setter
    @param thermotype The desired type: MCP9600_TYPE_K, MCP9600_TYPE_J,
    MCP9600_TYPE_T, MCP9600_TYPE_N, MCP9600_TYPE_S, MCP9600_TYPE_E,
    MCP9600_TYPE_B or MCP9600_TYPE_R
*/
/**************************************************************************/
void Adafruit_MCP9600::setThermocoupleType(MCP9600_ThemocoupleType thermotype) {
  // define the register
  Adafruit_I2CRegister sensorconfig =
      Adafruit_I2CRegister(i2c_dev, MCP9600_SENSORCONFIG, 1, MSBFIRST);
  Adafruit_I2CRegisterBits type =
      Adafruit_I2CRegisterBits(&sensorconfig, 3, 4); // # bits, bit_shift

  type.write(thermotype);
}

/**************************************************************************/
/*!
    @brief   The desired filter coefficient getter
    @returns How many readings we will be averaging, can be from 0-7
*/
/**************************************************************************/
uint8_t Adafruit_MCP9600::getFilterCoefficient(void) {
  // define the register
  Adafruit_I2CRegister sensorconfig =
      Adafruit_I2CRegister(i2c_dev, MCP9600_SENSORCONFIG, 1, MSBFIRST);
  Adafruit_I2CRegisterBits filter =
      Adafruit_I2CRegisterBits(&sensorconfig, 3, 0); // # bits, bit_shift

  return filter.read();
}

/**************************************************************************/
/*!
    @brief   The desired filter coefficient setter
    @param filtercount How many readings we will be averaging, can be from 0-7
*/
/**************************************************************************/
void Adafruit_MCP9600::setFilterCoefficient(uint8_t filtercount) {
  // define the register
  Adafruit_I2CRegister sensorconfig =
      Adafruit_I2CRegister(i2c_dev, MCP9600_SENSORCONFIG, 1, MSBFIRST);
  Adafruit_I2CRegisterBits filter =
      Adafruit_I2CRegisterBits(&sensorconfig, 3, 0); // # bits, bit_shift

  filter.write(filtercount);
}

/**************************************************************************/
/*!
    @brief  Getter for alert temperature setting
    @param  alert Which alert output we're getting, can be 1 to 4
    @return Floating point temperature in Centigrade
*/
/**************************************************************************/
float Adafruit_MCP9600::getAlertTemperature(uint8_t alert) {
  if ((alert < 1) || (alert > 4))
    return NAN; // invalid
  // define the register
  Adafruit_I2CRegister alerttemp = Adafruit_I2CRegister(
      i2c_dev, MCP9600_ALERTLIMIT_1 + alert - 1, 2, MSBFIRST);

  // read a signed 16 bit value
  int16_t therm = alerttemp.read();

  // convert to floating and shift to celsius
  float temp = therm;
  temp *= 0.0625; // 0.0625*C per LSB!
  return temp;
}

/**************************************************************************/
/*!
    @brief  Setter for alert temperature setting
    @param  alert Which alert output we're getting, can be 1 to 4
    @param  temp  Floating point temperature in Centigrade
*/
/**************************************************************************/
void Adafruit_MCP9600::setAlertTemperature(uint8_t alert, float temp) {
  if ((alert < 1) || (alert > 4))
    return; // invalid

  // define the register
  Adafruit_I2CRegister alerttemp = Adafruit_I2CRegister(
      i2c_dev, MCP9600_ALERTLIMIT_1 + alert - 1, 2, MSBFIRST);

  int16_t therm = temp / 0.0625; // 0.0625*C per LSB!
  alerttemp.write(therm);
}

/**************************************************************************/
/*!
    @brief Configure temperature alert
    @param alert Which alert output we're getting, can be 1 to 4
    @param enabled Whether this alert is on or off
    @param rising True if we want an alert when the temperature rises above.
    False for alert on falling below.
    @param alertColdJunction Whether the temperature we're watching is the
    internal chip temperature (true) or the thermocouple (false). Default is
    false
    @param activeHigh Whether output pin goes high on alert (true) or low
   (false)
    @param interruptMode Whether output pin latches on until we clear it (true)
    or comparator mode (false)
*/
/**************************************************************************/
void Adafruit_MCP9600::configureAlert(uint8_t alert, bool enabled, bool rising,
                                      bool alertColdJunction, bool activeHigh,
                                      bool interruptMode) {
  if ((alert < 1) || (alert > 4))
    return; // invalid
  // define the register
  Adafruit_I2CRegister alertconfig = Adafruit_I2CRegister(
      i2c_dev, MCP9600_ALERTCONFIG_1 + alert - 1, 1, MSBFIRST);

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

/**************************************************************************/
/*!
    @brief  Getter for status register
    @return 8-bit status, see datasheet for bits
*/
/**************************************************************************/
uint8_t Adafruit_MCP9600::getStatus(void) {
  // define the register
  Adafruit_I2CRegister status =
      Adafruit_I2CRegister(i2c_dev, MCP9600_STATUS, 1);

  return status.read();
}
