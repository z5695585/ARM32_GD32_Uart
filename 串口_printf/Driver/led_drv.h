#ifndef _LED_DRV_H_
#define _LED_DRV_H_

#include "stdint.h"
#define  LED1  0
#define  LED2  1
#define  LED3  2

/**
 * @description: LED初始化
 * @param  rcu_periph_enum periph，uint32_t gpio_periph，uint32_t pin
 * @return:
 */
void LedInit(void);
	

/**
 * @description: 打开LED
 * @param ledNo,0-2
 * @return:
 */
void TurnOnLed(uint8_t ledNo);


/**
 * @description: 关闭LED
 * @param ledNo,0-2
 * @return:
 */
void TurnOffLed(uint8_t ledNo);

/**
 * @description: LED状态反转
 * @param ledNo,0-2
 * @return:
 */
void ToggleLed(uint8_t ledNo);

#endif
