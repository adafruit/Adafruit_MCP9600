#include "Adafruit_MCP9601.h"

#define I2C_ADDRESS (0x67)

Adafruit_MCP9601 mcp;

/* Set and print ambient resolution */
Ambient_Resolution ambientRes = RES_ZERO_POINT_0625;

void setup()
{
    Serial.begin(115200);
    while (!Serial) {
      delay(10);
    }
    Serial.println("Adafruit MCP9601 test");

    /* Initialise the driver with I2C_ADDRESS and the default I2C bus. */
    if (! mcp.begin(I2C_ADDRESS)) {
        Serial.println("Sensor not found. Check wiring!");
        while (1);
    }

  Serial.println("Found MCP9601!");

  /* Set and print ambient resolution */
  mcp.setAmbientResolution(ambientRes);
  Serial.print("Ambient Resolution set to: ");
  switch (ambientRes) {
    case RES_ZERO_POINT_25:    Serial.println("0.25°C"); break;
    case RES_ZERO_POINT_125:   Serial.println("0.125°C"); break;
    case RES_ZERO_POINT_0625:  Serial.println("0.0625°C"); break;
    case RES_ZERO_POINT_03125: Serial.println("0.03125°C"); break;
  }

  mcp.setADCresolution(MCP9600_ADCRESOLUTION_18);
  Serial.print("ADC resolution set to ");
  switch (mcp.getADCresolution()) {
    case MCP9600_ADCRESOLUTION_18:   Serial.print("18"); break;
    case MCP9600_ADCRESOLUTION_16:   Serial.print("16"); break;
    case MCP9600_ADCRESOLUTION_14:   Serial.print("14"); break;
    case MCP9600_ADCRESOLUTION_12:   Serial.print("12"); break;
  }
  Serial.println(" bits");

  mcp.setThermocoupleType(MCP9600_TYPE_K);
  Serial.print("Thermocouple type set to ");
  switch (mcp.getThermocoupleType()) {
    case MCP9600_TYPE_K:  Serial.print("K"); break;
    case MCP9600_TYPE_J:  Serial.print("J"); break;
    case MCP9600_TYPE_T:  Serial.print("T"); break;
    case MCP9600_TYPE_N:  Serial.print("N"); break;
    case MCP9600_TYPE_S:  Serial.print("S"); break;
    case MCP9600_TYPE_E:  Serial.print("E"); break;
    case MCP9600_TYPE_B:  Serial.print("B"); break;
    case MCP9600_TYPE_R:  Serial.print("R"); break;
  }
  Serial.println(" type");

  mcp.setFilterCoefficient(3);
  Serial.print("Filter coefficient value set to: ");
  Serial.println(mcp.getFilterCoefficient());

  mcp.setAlertTemperature(1, 30);
  Serial.print("Alert #1 temperature set to ");
  Serial.println(mcp.getAlertTemperature(1));
  mcp.configureAlert(1, true, true);  // alert 1 enabled, rising temp

  mcp.enable(true);

  Serial.println(F("------------------------------"));
}

void loop()
{
  uint8_t status = mcp.getStatus();
  Serial.print("MCP Status: 0x"); 
  Serial.print(status, HEX);  
  Serial.print(": ");
  if (status & MCP9601_STATUS_OPENCIRCUIT) { 
    Serial.println("Thermocouple open!"); 
    return; // don't continue, since there's no thermocouple
  }
  if (status & MCP9601_STATUS_SHORTCIRCUIT) { 
    Serial.println("Thermocouple shorted to ground!"); 
    return; // don't continue, since the sensor is not working
  }
  if (status & MCP960X_STATUS_ALERT1) { Serial.print("Alert 1, "); }
  if (status & MCP960X_STATUS_ALERT2) { Serial.print("Alert 2, "); }
  if (status & MCP960X_STATUS_ALERT3) { Serial.print("Alert 3, "); }
  if (status & MCP960X_STATUS_ALERT4) { Serial.print("Alert 4, "); }
  Serial.println();
  
  Serial.print("Hot Junction: "); Serial.println(mcp.readThermocouple());
  Serial.print("Cold Junction: "); Serial.println(mcp.readAmbient());
  Serial.print("ADC: "); Serial.print(mcp.readADC() * 2); Serial.println(" uV");





  delay(1000);
}
