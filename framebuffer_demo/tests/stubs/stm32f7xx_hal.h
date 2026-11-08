#pragma once
#include <stdint.h>
#define __weak
#define I2C_MEMADD_SIZE_8BIT 1
typedef struct { int unused; } I2C_HandleTypeDef;
typedef enum { HAL_OK = 0, HAL_ERROR = 1 } HAL_StatusTypeDef;
#ifdef __cplusplus
extern "C" {
#endif
HAL_StatusTypeDef HAL_I2C_Mem_Read(I2C_HandleTypeDef*, uint16_t, uint16_t, uint16_t, uint8_t*, uint16_t, uint32_t);
HAL_StatusTypeDef HAL_I2C_Mem_Write(I2C_HandleTypeDef*, uint16_t, uint16_t, uint16_t, uint8_t*, uint16_t, uint32_t);
HAL_StatusTypeDef HAL_I2C_DeInit(I2C_HandleTypeDef*);
void HAL_Delay(uint32_t);
#ifdef __cplusplus
}
#endif
