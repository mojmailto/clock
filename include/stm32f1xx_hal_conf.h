#ifndef __STM32F1xx_HAL_CONF_H
#define __STM32F1xx_HAL_CONF_H

#define HAL_MODULE_ENABLED
#define HAL_GPIO_MODULE_ENABLED
#define HAL_RCC_MODULE_ENABLED
#define HAL_I2C_MODULE_ENABLED
#define HAL_CORTEX_MODULE_ENABLED
#define HAL_DMA_MODULE_ENABLED
#define HAL_FLASH_MODULE_ENABLED
#define HAL_PWR_MODULE_ENABLED

#define HSE_VALUE    ((uint32_t)8000000)
#define HSE_STARTUP_TIMEOUT    ((uint32_t)100)
#define HSI_VALUE    ((uint32_t)8000000)
#define LSI_VALUE    ((uint32_t)40000)
#define LSE_VALUE    ((uint32_t)32768)
#define VDD_VALUE    ((uint32_t)3300)
#define TICK_INT_PRIORITY    ((uint32_t)0)
#define USE_RTOS    0
#define PREFETCH_ENABLE    1

#include "stm32f1xx_hal_gpio.h"
#include "stm32f1xx_hal_rcc.h"
#include "stm32f1xx_hal_i2c.h"
#include "stm32f1xx_hal_dma.h"
#include "stm32f1xx_hal_cortex.h"
#include "stm32f1xx_hal_flash.h"
#include "stm32f1xx_hal_pwr.h"

#ifdef  USE_FULL_ASSERT
  #define assert_param(expr) ((expr) ? (void)0U : assert_failed((uint8_t *)__FILE__, __LINE__))
  void assert_failed(uint8_t* file, uint32_t line);
#else
  #define assert_param(expr) ((void)0U)
#endif

#endif /* __STM32F1xx_HAL_CONF_H */
