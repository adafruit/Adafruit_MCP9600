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
*/
/**************************************************************************/
#ifndef __ADAFRUIT_MCP9600_H__
#define __ADAFRUIT_MCP9600_H__

#include <Adafruit_I2CDevice.h>
#include <Adafruit_I2CRegister.h>
#include <Arduino.h>
#include <Wire.h>

/** Default MCP9600 I2C address. */
#define MCP9600_I2CADDR_DEFAULT 0x67 ///< I2C address

#define MCP9600_HOTJUNCTION 0x00   ///< Hot junction temperature T_H
#define MCP9600_JUNCTIONDELTA 0x01 ///< Hot/Cold junction delta
#define MCP9600_COLDJUNCTION 0x02  ///< Cold junction temperature T_C
#define MCP9600_RAWDATAADC 0x03    ///< The 'raw' uV reading
#define MCP9600_STATUS 0x04        ///< Current device status
#define MCP9600_SENSORCONFIG 0x05  ///< Configuration for thermocouple type
#define MCP9600_DEVICECONFIG 0x06  ///< Device config like sleep mode
#define MCP9600_DEVICEID 0x20      ///< Device ID/Revision
#define MCP9600_ALERTCONFIG_1 0x08 ///< The first alert's config
#define MCP9600_ALERTHYST_1 0x0C   ///< The first alert's hystersis
#define MCP9600_ALERTLIMIT_1 0x10  ///< the first alert's limitval

#define MCP960X_STATUS_ALERT1 0x01     ///< Bit flag for alert 1 status
#define MCP960X_STATUS_ALERT2 0x02     ///< Bit flag for alert 2 status
#define MCP960X_STATUS_ALERT3 0x04     ///< Bit flag for alert 3 status
#define MCP960X_STATUS_ALERT4 0x08     ///< Bit flag for alert 4 status
#define MCP960X_STATUS_INPUTRANGE 0x10 ///< Bit flag for input range
#define MCP960X_STATUS_THUPDATE 0x40   ///< Bit flag for TH update
#define MCP960X_STATUS_BURST 0x80      ///< Bit flag for burst complete

/*! The possible Thermocouple types */
typedef enum _themotype {
  MCP9600_TYPE_K,
  MCP9600_TYPE_J,
  MCP9600_TYPE_T,
  MCP9600_TYPE_N,
  MCP9600_TYPE_S,
  MCP9600_TYPE_E,
  MCP9600_TYPE_B,
  MCP9600_TYPE_R,
} MCP9600_ThemocoupleType;

/*! The possible ADC resolution settings */
typedef enum _resolution {
  MCP9600_ADCRESOLUTION_18,
  MCP9600_ADCRESOLUTION_16,
  MCP9600_ADCRESOLUTION_14,
  MCP9600_ADCRESOLUTION_12,
} MCP9600_ADCResolution;

/*! The possible Ambient resolutions */
typedef enum {
  RES_ZERO_POINT_25 = 0b00,   ///< 0.25°C
  RES_ZERO_POINT_125 = 0b01,  ///< 0.125°C
  RES_ZERO_POINT_0625 = 0b10, ///< 0.0625°C
  RES_ZERO_POINT_03125 = 0b11 ///< 0.03125°C
} Ambient_Resolution;

/**************************************************************************/
/*!
    @brief  MCP9600 driver.
*/
/**************************************************************************/
class Adafruit_MCP9600 {
public:
  Adafruit_MCP9600();
  boolean begin(uint8_t i2c_addr = MCP9600_I2CADDR_DEFAULT,
                TwoWire *theWire = &Wire);

  float readThermocouple(void);
  float readAmbient(void);

  void enable(bool flag);
  bool enabled(void);

  MCP9600_ThemocoupleType getThermocoupleType(void);
  void setThermocoupleType(MCP9600_ThemocoupleType);

  uint8_t getFilterCoefficient(void);
  void setFilterCoefficient(uint8_t);

  void setADCresolution(MCP9600_ADCResolution resolution);
  MCP9600_ADCResolution getADCresolution(void);
  int32_t readADC(void);

  void setAmbientResolution(Ambient_Resolution res_value);

  void setAlertTemperature(uint8_t alert, float temp);
  float getAlertTemperature(uint8_t alert);
  void configureAlert(uint8_t alert, bool enabled, bool rising,
                      bool alertColdJunction = false, bool activeHigh = false,
                      bool interruptMode = false);

  uint8_t getStatus(void);

protected:
  uint8_t _device_id = 0; ///< The DEVICE ID high byte for MCP9600 or MCP9601
private:
  Adafruit_I2CDevice *i2c_dev;
  Adafruit_I2CRegister *_config_reg;
};

#endif // #define __ADAFRUIT_MCP9600_H__
