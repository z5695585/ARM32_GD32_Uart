#include <stdint.h>
#include "gd32f30x.h"
#include "gd32f30x_rcu.h"
#include "gd32f30x_gpio.h"

#define LED_NUM_MAX (sizeof(g_ledList) / sizeof(g_ledList[0]))

typedef struct 
{
    rcu_periph_enum rcu_periph;
    uint32_t gpio_periph;
    uint32_t pin;
} ledPara;

static ledPara g_ledList[] = {
    {RCU_GPIOA, GPIOA, GPIO_PIN_8},
	{RCU_GPIOE, GPIOE, GPIO_PIN_6},
	{RCU_GPIOF, GPIOF, GPIO_PIN_6}
};

/**
 * @description: LED初始化
 * @param  rcu_periph_enum periph，uint32_t gpio_periph，uint32_t pin
 * @return:
 */
void LedInit(void)
{
	for (uint8_t i = 0; i < LED_NUM_MAX; i++)
    {
        rcu_periph_clock_enable(g_ledList[i].rcu_periph);
		gpio_init(g_ledList[i].gpio_periph, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, g_ledList[i].pin);
		gpio_bit_reset(g_ledList[i].gpio_periph,g_ledList[i].pin);		
    }    
}

/**
 * @description: 打开LED
 * @param ledNo,0-2
 * @return:
 */
void TurnOnLed(uint8_t ledNo)
{
	gpio_bit_set(g_ledList[ledNo].gpio_periph,g_ledList[ledNo].pin);
}

/**
 * @description: 关闭LED
 * @param ledNo,0-2
 * @return:
 */
void TurnOffLed(uint8_t ledNo)
{
	gpio_bit_reset(g_ledList[ledNo].gpio_periph,g_ledList[ledNo].pin);
}

/**
 * @description: LED状态反转
 * @param ledNo,0-2
 * @return:
 */
void ToggleLed(uint8_t ledNo)
{
	if (ledNo >= LED_NUM_MAX)
	{
		return;
	}
	FlagStatus bit_state;
	bit_state = gpio_input_bit_get(g_ledList[ledNo].gpio_periph, g_ledList[ledNo].pin);
	bit_state = (FlagStatus)(1 - bit_state);
	gpio_bit_write(g_ledList[ledNo].gpio_periph, g_ledList[ledNo].pin, bit_state);	
}


