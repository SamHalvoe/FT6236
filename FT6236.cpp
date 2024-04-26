/*
This is a library for the FT6236 touchscreen controller by FocalTech.

The FT6236 and FT6236u work the same way.

A lot of this library is originally written by Limor Fried/Ladyada.

Because Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing
products from Adafruit!

@section author Author
Written by Limor Fried/Ladyada for Adafruit Industries.

@section license License
MIT license, all text above must be included in any redistribution
*/

#include "FT6236.h"

/* ---- Class TouchPoint ---- */

TouchPoint::TouchPoint(uint16_t in_x, uint16_t in_y) : pm_x(in_x), pm_y(in_y)
{}

/* == comparator between two points */
bool TouchPoint::operator==(const TouchPoint& in_point)
{
    return ((in_point.pm_x == pm_x) && (in_point.pm_y == pm_y));
}

/* != comparator between two points */
bool TouchPoint::operator!=(const TouchPoint& in_point)
{
    return ((in_point.pm_x != pm_x) || (in_point.pm_y != pm_y));
}

/* ---- Class FT6236 ---- */

FT6236::FT6236(TwoWire& wire) : ftWire(wire)
{
    touches = FT6236_INVALID_STATE;

    for (uint8_t i = 0; i < 2; i++)
    {
        touchX[i] = FT6236_INVALID_STATE;
        touchY[i] = FT6236_INVALID_STATE;
        touchID[i] = FT6236_INVALID_STATE;
    }
}

/* Start I2C and check if the FT6236 is found. */
boolean FT6236::begin(uint8_t thresh)
{
    ftWire.begin();

    // Adjust threshold
    writeRegister8(FT6236_REG_THRESHHOLD, thresh);

    //Check if our chip has the correct Vendor ID
    if (readRegister8(FT6236_REG_VENDID) != FT6236_VENDID)
    {
        return false;
    }
    
    //Check if our chip has the correct Chip ID.
    uint8_t id = readRegister8(FT6236_REG_CHIPID);

    if ((id != FT6236_CHIPID) && (id != FT6236U_CHIPID) && (id != FT6206_CHIPID))
    {
        return false;
    }

    return true;
}

/* Returns the number of touches */
uint8_t FT6236::touched(void)
{
    uint8_t n = readRegister8(FT6236_REG_NUMTOUCHES);

    return (n > 2) ? 0 : n;
}

/* Get a touch point */
TouchPoint FT6236::getPoint(uint8_t n)
{
    readData();

    if ((touches == 0) || (n > 1))
    {
        return TouchPoint(FT6236_INVALID_STATE, FT6236_INVALID_STATE);
    }
    else
    {
        return TouchPoint(touchX[n], touchY[n]);
    }
}

void FT6236::readData(void)
{

    uint8_t i2cdat[16];
    ftWire.beginTransmission(FT6236_ADDR);
    ftWire.write((byte)0);
    ftWire.endTransmission();

    ftWire.requestFrom((byte)FT6236_ADDR, (byte)16);
    for (uint8_t i = 0; i < 16; i++)
        i2cdat[i] = ftWire.read();

    touches = i2cdat[0x02];
    if ((touches > 2) || (touches == 0))
    {
        touches = 0;

        for (uint8_t i = 0; i < 2; i++)
        {
            touchX[i] = FT6236_INVALID_STATE;
            touchY[i] = FT6236_INVALID_STATE;
            touchID[i] = FT6236_INVALID_STATE;
        }
    }
    else
    {
        for (uint8_t i = 0; i < 2; i++)
        {
            touchID[i] = i2cdat[0x05 + i * 6] >> 4;

            if (touchID[i] > 1)
            {
                touchX[i] = FT6236_INVALID_STATE;
                touchY[i] = FT6236_INVALID_STATE;
                touchID[i] = FT6236_INVALID_STATE;
                continue;
            }

            touchX[i] = i2cdat[0x03 + i * 6] & 0x0F;
            touchX[i] <<= 8;
            touchX[i] |= i2cdat[0x04 + i * 6];
            touchY[i] = i2cdat[0x05 + i * 6] & 0x0F;
            touchY[i] <<= 8;
            touchY[i] |= i2cdat[0x06 + i * 6];
        }
    }
}

/* Reading a byte from a register */
uint8_t FT6236::readRegister8(uint8_t reg)
{
    uint8_t x;

    ftWire.beginTransmission(FT6236_ADDR);
    ftWire.write((byte)reg);
    ftWire.endTransmission();

    ftWire.requestFrom((byte)FT6236_ADDR, (byte)1);
    x = ftWire.read();

    return x;
}

/* Writing a byte to a register */
void FT6236::writeRegister8(uint8_t reg, uint8_t val)
{

    ftWire.beginTransmission(FT6236_ADDR);
    ftWire.write((byte)reg);
    ftWire.write((byte)val);
    ftWire.endTransmission();
}

/* Debug */
void FT6236::debug(void)
{
    Serial.print("Vend ID: 0x");
    Serial.println(readRegister8(FT6236_REG_VENDID), HEX);
    Serial.print("Chip ID: 0x");
    Serial.println(readRegister8(FT6236_REG_CHIPID), HEX);
    Serial.print("Firm V: ");
    Serial.println(readRegister8(FT6236_REG_FIRMVERS));
    Serial.print("Point Rate Hz: ");
    Serial.println(readRegister8(FT6236_REG_POINTRATE));
    Serial.print("Thresh: ");
    Serial.println(readRegister8(FT6236_REG_THRESHHOLD));
}
