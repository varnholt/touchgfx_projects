#include <stm32touchcontroller.hpp>
#include <stm32f7xx_hal.h>
#include <ft6x06.h>
#include <array>
#include <cstdlib>
#include <iostream>

namespace
{
std::array<uint8_t, 256> registers = {};
int failRegister = -1;
int recoveries = 0;
void requireHardware(bool condition, const char* message)
{
    if (!condition) { std::cerr << "FAIL: " << message << '\n'; std::exit(1); }
}
void rawPosition(int x, int y)
{
    // Landscape setup swaps raw axes and inverts the resulting Y.
    const int rawX = 479 - y, rawY = x;
    registers[FT6206_P1_XH_REG] = static_cast<uint8_t>(rawX >> 8);
    registers[FT6206_P1_XL_REG] = static_cast<uint8_t>(rawX);
    registers[FT6206_P1_YH_REG] = static_cast<uint8_t>(rawY >> 8);
    registers[FT6206_P1_YL_REG] = static_cast<uint8_t>(rawY);
}
}

extern "C"
{
I2C_HandleTypeDef hi2c4 = {};
void MX_I2C4_Init() { ++recoveries; }
void HAL_Delay(uint32_t) {}
HAL_StatusTypeDef HAL_I2C_DeInit(I2C_HandleTypeDef*) { return HAL_OK; }
HAL_StatusTypeDef HAL_I2C_Mem_Read(I2C_HandleTypeDef*, uint16_t, uint16_t reg,
                                 uint16_t, uint8_t* buffer, uint16_t length, uint32_t)
{
    if (reg == failRegister) return HAL_ERROR; // Deliberately leave the buffer untouched.
    for (uint16_t i = 0; i < length; ++i) buffer[i] = registers[(reg + i) & 255];
    return HAL_OK;
}
HAL_StatusTypeDef HAL_I2C_Mem_Write(I2C_HandleTypeDef*, uint16_t, uint16_t, uint16_t,
                                  uint8_t*, uint16_t, uint32_t) { return HAL_OK; }
}

void runHardwareTouchTests()
{
    registers[FT6206_CHIP_ID_REG] = FT6206_ID_VALUE;
    STM32TouchController controller;
    controller.init();
    int32_t x = -1, y = -1;
    registers[FT6206_TD_STAT_REG] = 1;
    rawPosition(2, 2);
    requireHardware(controller.sampleTouch(x, y) && x == 2 && y == 2, "hardware first contact");
    registers[FT6206_TD_STAT_REG] = 0;
    requireHardware(!controller.sampleTouch(x, y), "hardware release");
    registers[FT6206_TD_STAT_REG] = 1;
    rawPosition(3, 3);
    requireHardware(controller.sampleTouch(x, y) && x == 3 && y == 3, "hardware new contact jitter anchor");
    failRegister = FT6206_P1_XH_REG;
    requireHardware(!controller.sampleTouch(x, y) && recoveries > 0, "failed XY read must not become a touch");
    failRegister = -1;
    rawPosition(4, 4);
    requireHardware(controller.sampleTouch(x, y) && x == 4 && y == 4, "contact after I2C recovery");
    failRegister = FT6206_TD_STAT_REG;
    requireHardware(!controller.sampleTouch(x, y), "failed contact count read must not become a touch");
    failRegister = -1;
    rawPosition(900, 20);
    requireHardware(!controller.sampleTouch(x, y), "out-of-range hardware coordinate");
}
