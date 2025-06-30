#include <stdint.h>
#include "gd32f30x.h"
#include "core_cm4.h"
#include "led_drv.h"

#define KEY_NUM_MAX (sizeof(g_keyList) / sizeof(g_keyList[0]))

typedef struct 
{
    rcu_periph_enum rcu_periph;
    uint32_t gpio_periph;
    uint32_t pin;
	uint8_t output_port;   	//用于gpio_exti_source_select函数的参数
	uint8_t output_pin;		//用于gpio_exti_source_select函数的参数
} keyPara;

static keyPara g_keyList[] = {
    {RCU_GPIOA, GPIOA, GPIO_PIN_0, GPIO_PORT_SOURCE_GPIOA, GPIO_PIN_SOURCE_0},
	{RCU_GPIOG, GPIOG, GPIO_PIN_13, GPIO_PORT_SOURCE_GPIOA, GPIO_PIN_SOURCE_0},
	{RCU_GPIOG, GPIOG, GPIO_PIN_14, GPIO_PORT_SOURCE_GPIOA, GPIO_PIN_SOURCE_0},
	{RCU_GPIOG, GPIOG, GPIO_PIN_15, GPIO_PORT_SOURCE_GPIOA, GPIO_PIN_SOURCE_0}
};

/**
***********************************************************
* @brief 按键初始化
* @param
* @return 
***********************************************************
*/
void KeyDrvInit(void)
{
	rcu_periph_clock_enable(RCU_AF);	//使能EXTI时钟
	
	/*初始化按键的GPIO*/
	for (uint8_t i = 0; i < KEY_NUM_MAX; i++)
    {
        rcu_periph_clock_enable(g_keyList[i].rcu_periph);
		gpio_init(g_keyList[i].gpio_periph, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_2MHZ, g_keyList[i].pin);
		gpio_exti_source_select(g_keyList[i].output_port, g_keyList[i].output_pin);
    }
		
	/*初始化EXTI中断*/	
	//选择连接到EXTI线的GPIO
	exti_init(EXTI_0, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
	exti_init(EXTI_13, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
	exti_init(EXTI_14, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
	exti_init(EXTI_15, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
	//清除EXTI标志
	exti_flag_clear(EXTI_0);
	exti_flag_clear(EXTI_13);
	exti_flag_clear(EXTI_14);
	exti_flag_clear(EXTI_15);
	
	
	// 配置NVIC
	nvic_irq_enable(EXTI0_IRQn, 0, 0);
	nvic_irq_enable(EXTI10_15_IRQn, 1, 0);
	
	//使能EXTI
	exti_interrupt_enable(EXTI_0);
	exti_interrupt_enable(EXTI_13);
	exti_interrupt_enable(EXTI_14);
	exti_interrupt_enable(EXTI_15);
}

/***********************************************************
* @brief EXTI0中断服务函数，对应PA0
* @param
* @return 
***********************************************************
*/
void EXTI0_IRQHandler(void)
{
	if (exti_interrupt_flag_get(EXTI_0) != RESET)
	{
		ToggleLed(LED1);
	}
	exti_flag_clear(EXTI_0);
	
}


