/*!
 * @brief A rotary encoder expansion From M5Stack
 * @copyright Copyright (c) 2022 by M5Stack[https://m5stack.com]
 *
 * @Links [Unit Encoder](https://docs.m5stack.com/en/unit/encoder)
 * @version  V0.0.2
 * @date  2022-07-11
 */
#ifndef _UNIT_ENCODER_H_
#define _UNIT_ENCODER_H_

#include "Arduino.h"
#include "Wire.h"
#include "pins_arduino.h"

#define ENCODER_ADDR       0x40
#define ENCODER_REG        0x10
#define BUTTON_REG         0x20
#define RGB_LED_REG        0x30
#define RESET_REG          0x40
#define INC_ENCODER_REG    0x50
#define BOOTLOADER_VERSION_REG                            0xFC
#define JUMP_TO_BOOTLOADER_REG                            0xFD
#define FIRMWARE_VERSION_REG                              0xFE
#define I2C_ADDRESS_REG                                   0xFF

class Unit_Encoder {
   private:
    uint8_t _addr;
    TwoWire* _wire;
    uint8_t _scl;
    uint8_t _sda;
    uint32_t _speed;
    void writeBytes(uint8_t addr, uint8_t reg, uint8_t* buffer, uint8_t length);
    void readBytes(uint8_t addr, uint8_t reg, uint8_t* buffer, uint8_t length);

   public:
    bool begin(TwoWire* wire = &Wire, uint8_t addr = ENCODER_ADDR,
               uint8_t sda = 21, uint8_t scl = 22, uint32_t speed = 400000U);
    int32_t getEncoderValue(void);
    int32_t getIncEncoderValue(void);
    bool getButtonStatus(void);
    void setLEDColor(uint32_t color, uint8_t index = 0);
    uint32_t getLEDColor(void);
    void setEncoderValue(int32_t encoder);
    void resetEncoder(void);
    bool getDevStatus(void);
    uint8_t getBootloaderVersion(void);
    uint8_t getFirmwareVersion(void);
    uint8_t setI2CAddress(uint8_t addr);
    uint8_t getI2CAddress(void);
    void jumpBootloader(void);
};

#endif
