/*!
 * FT6236.h
 */

#ifndef FT6236_H
#define FT6236_H

#include "Arduino.h"
#include <Wire.h>

#define FT6236_ADDR 0x38           // I2C address
#define FT6236_G_FT5201ID 0xA8     // FocalTech's panel ID
#define FT6236_REG_NUMTOUCHES 0x02 // Number of touch points

#define FT6236_NUM_X 0x33 // Touch X position
#define FT6236_NUM_Y 0x34 // Touch Y position

#define FT6236_REG_MODE 0x00        // Device mode, either WORKING or FACTORY
#define FT6236_REG_CALIBRATE 0x02   // Calibrate mode
#define FT6236_REG_WORKMODE 0x00    // Work mode
#define FT6236_REG_FACTORYMODE 0x40 // Factory mode
#define FT6236_REG_THRESHHOLD 0x80  // Threshold for touch detection
#define FT6236_REG_POINTRATE 0x88   // Point rate
#define FT6236_REG_FIRMVERS 0xA6    // Firmware version
#define FT6236_REG_CHIPID 0xA3      // Chip selecting
#define FT6236_REG_VENDID 0xA8      // FocalTech's panel ID

#define FT6236_VENDID 0x11  // FocalTech's panel ID
#define FT6206_CHIPID 0x06  // FT6206 ID
#define FT6236_CHIPID 0x36  // FT6236 ID
#define FT6236U_CHIPID 0x64 // FT6236U ID

#define FT6236_DEFAULT_THRESHOLD 128 // Default threshold for touch detection

const uint16_t FT6236_INVALID_STATE = 0xFFFF;

struct TouchPoint
{
  public:
    TouchPoint() = default;
    TouchPoint(uint16_t in_x, uint16_t in_y);

    bool operator==(const TouchPoint& in_point);
    bool operator!=(const TouchPoint& in_point);

  public:
    uint16_t pm_x = FT6236_INVALID_STATE;
    uint16_t pm_y = FT6236_INVALID_STATE;
};

class FT6236
{
  public:
    FT6236(TwoWire& wire = Wire);
    boolean begin(uint8_t thresh = FT6236_DEFAULT_THRESHOLD);
    void debug();
    TouchPoint getPoint(uint8_t n);
    uint8_t touched();
    void readData();
    uint8_t touches;
    uint16_t touchX[2], touchY[2], touchID[2];

  private:
    void writeRegister8(uint8_t reg, uint8_t val);
    uint8_t readRegister8(uint8_t reg);

    TwoWire& ftWire;
};

#endif
