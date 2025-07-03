#include <stdint.h>
#include "gd32f30x.h"
#include "core_cm4.h"
#include "gd32f30x_rcu.h"
#include "led_drv.h"

volatile static uint64_t g_sysRunTime;		

/**
 * @description: Systick初始化
 * @param  
 * @return:
 */
void SystickInit(void)
{
	SysTick_Config(rcu_clock_freq_get(CK_AHB) / 1000);
}

/**
 * @description: 获取系统运行时间
 * @param  
 * @return:
 */
uint64_t GetSysRunTime(void)
{
	return g_sysRunTime;
}

/**
 * @description: Systick中断服务函数
 * @param  
 * @return:
 */
void SysTick_Handler(void)
{
	g_sysRunTime++;
}
