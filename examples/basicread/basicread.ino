#include <Wire.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_I2CRegister.h>
#include "Adafruit_MCP9600.h"

#define I2C_ADDRESS 0x53

Adafruit_MCP9600 mcp;
Adafruit_I2CDevice i2c_dev = Adafruit_I2CDevice(I2C_ADDRESS);

void
setup()
{
    Serial.begin(9600);
    Serial.println("MCP9600 HW test");

    /* Initialise the driver with I2C_ADDRESS and the default I2C bus. */
    if (! mcp.begin(&i2c_dev)) {
        Serial.println("Sensor not found. Check wiring!");
        while (1) {
            delay(10);
        }
    }

    Serial.println("Found MCP9600!");
}

void
loop()
{
    Serial.println("ToDo");
    delay(100);
}
