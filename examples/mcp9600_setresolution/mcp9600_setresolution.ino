#include <Adafruit_MCP9600.h>

Adafruit_MCP9600 mcp;

// New variables for resolutions
Ambient_Resolution ambientRes = RES_ZERO_POINT_0625;
Thermocouple_Resolution thermocoupleRes = RES_14_BIT;

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);

  if (!mcp.begin(0x67)) {
    Serial.println("Failed to find MCP9600 chip");
    while (1) { delay(10); }
  }

  // Set the resolutions
  mcp.setAmbientResolution(ambientRes);
  mcp.setThermocoupleResolution(thermocoupleRes);

  Serial.println("MCP9600 ready.");
}

void loop() {
  float ambientTemp = mcp.readAmbient();
  float thermocoupleTemp = mcp.readThermocouple();

  Serial.print("Ambient Temp: ");
  Serial.print(ambientTemp, 2);
  Serial.print(" C, Thermocouple Temp: ");
  Serial.print(thermocoupleTemp, 2);
  Serial.println(" C");

  delay(1000);
}
