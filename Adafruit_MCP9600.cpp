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

uint32_t Adafruit_MCP9600::readRegister(uint8_t reg_addr, uint8_t bytes) {
  uint8_t buf[4] = {reg_addr};
  uint32_t ret = 0;

  // use STOP!
  if (! i2c_dev->write_then_read(buf, 1, buf, bytes, true)) {
    return ret;
  }
  delay(10);
  ret = 0;
  for (int i=0; i<bytes; i++) {
    ret <<= 8;
    ret |= buf[i];
  }
  Serial.println(ret, HEX);
  return ret;
}

bool Adafruit_MCP9600::writeRegister(uint8_t reg_addr, uint8_t *vals, uint8_t bytes) {
  uint8_t buf[5] = {reg_addr, 0, 0, 0, 0};

  if (bytes == 2) {
    buf[1] = vals[1];
    buf[2] = vals[0];
  }
  if (bytes == 1) {
    buf[1] = vals[0];
  }

  // use STOP!
  return i2c_dev->write(buf, 1+bytes);
}

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
   * be 0x40 or 0x41 */
  if (readRegister(MCP9600_DEVICEID, 2) >> 8 != _device_id) {
    return false;
  }

  // define the config register
  uint8_t conf = 0x80;
  writeRegister(MCP9600_DEVICECONFIG, &conf, 1);
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

  uint8_t status = readRegister(MCP9600_STATUS, 1);
  status &= 0x3F; // clear top 2 bits
  writeRegister(MCP9600_STATUS, &status, 1);

  uint8_t config = readRegister(MCP9600_DEVICECONFIG, 1);
  config &= ~0b11; // clear config mode bits
  config |= 0b10; // set burst mode

  delay(200);
  Serial.print("midstatus: 0x");
  Serial.println(readRegister(MCP9600_STATUS, 1), HEX);
  delay(200);
  Serial.print("endstatus: 0x");
  Serial.println(readRegister(MCP9600_STATUS, 1), HEX);

  // define the register
  // read a signed 16 bit value
  int16_t therm = (int16_t)readRegister(MCP9600_HOTJUNCTION, 2);

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
  // read a signed 16 bit value
  int16_t cold = (int16_t)readRegister(MCP9600_COLDJUNCTION, 2);
  
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
  uint8_t status = readRegister(MCP9600_DEVICECONFIG, 1);
  status &= ~0x03; // mask off bottom 2 bits

  if (!flag) { // sleep mode
    status |= 0x01;
  } else {
    status |= 0x00;
  }
  writeRegister(MCP9600_DEVICECONFIG, &status, 1);
}

/**************************************************************************/
/*!
    @brief  Whether the sensor is enabled and working or in sleep mode
    @returns True if in awake mode, False if in sleep mode
*/
/**************************************************************************/
bool Adafruit_MCP9600::enabled(void) {
  return !(readRegister(MCP9600_DEVICECONFIG, 1) & 0x3); // mask off bottom 2 bits
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

  uint8_t status = readRegister(MCP9600_DEVICECONFIG, 1);
  status &= ~0b01100000; // mask off the 2 bits
  status |= resolution << 5;
  
  writeRegister(MCP9600_DEVICECONFIG, &status, 1);
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
  return (MCP9600_ADCResolution)((readRegister(MCP9600_DEVICECONFIG, 1) >> 5) & 0x3);
}

/**************************************************************************/
/*!
    @brief  Read the raw ADC voltage, say for self calculating a temperature
    @returns The 32-bit signed value from the ADC DATA register
*/
/**************************************************************************/
int32_t Adafruit_MCP9600::readADC(void) {
  // define the register
  uint32_t reading = readRegister(MCP9600_RAWDATAADC, 3);
 
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

  int16_t therm = temp / 0.0625; // 0.0625*C per LSB!
  uint8_t buf[2] = {(uint8_t)(therm >> 8), (uint8_t)(therm & 0xFF)};

  writeRegister(MCP9600_ALERTLIMIT_1 + alert - 1, buf, 2);
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

  writeRegister(MCP9600_ALERTCONFIG_1 + alert - 1, &c, 1);
}


/**************************************************************************/
/*!
    @brief  Getter for status register
    @return 8-bit status, see datasheet for bits
*/
/**************************************************************************/
uint8_t Adafruit_MCP9600::getStatus(void) {
  return readRegister(MCP9600_STATUS, 1);
}
