/**************************************************************************/
/**
  @file     Adafruit_MCP9601.h

  Author: K. Townsend (Adafruit Industries)
  License: BSD (see license.txt)

  This is a library for the Adafruit MCP9601 I2C Thermocouple breakout board
  ----> http://www.adafruit.com/products/

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!
*/
/**************************************************************************/
#ifndef __ADAFRUIT_MCP9601_H__
#define __ADAFRUIT_MCP9601_H__

#include <Adafruit_MCP9600.h>

#define MCP9601_STATUS_OPENCIRCUIT 0x10  ///< Bit flag for open circuit
#define MCP9601_STATUS_SHORTCIRCUIT 0x20 ///< Bit flag for short circuit

/**************************************************************************/
/*!
    @brief  MCP9601 driver.
*/
/**************************************************************************/

class Adafruit_MCP9601 : public Adafruit_MCP9600 {
public:
  Adafruit_MCP9601();
};

#endif // #define __ADAFRUIT_MCP9601_H__
