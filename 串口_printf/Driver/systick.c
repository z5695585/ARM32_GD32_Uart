#include <stdint.h>
#include "gd32f30x.h"
#include "core_cm4.h"
#include "gd32f30x_rcu.h"
#include "led_drv.h"

volatile static uint64_t g_sysRunTime;		

/**
 * @description: Systick��ʼ��
 * @param  
 * @return:
 */
void SystickInit(void)
{
	SysTick_Config(rcu_clock_freq_get(CK_AHB) / 1000);
}

/**
 * @description: ��ȡϵͳ����ʱ��
 * @param  
 * @return:
 */
uint64_t GetSysRunTime(void)
{
	return g_sysRunTime;
}

/**
 * @description: Systick�жϷ�����
 * @param  
 * @return:
 */
void SysTick_Handler(void)
{
	g_sysRunTime++;
}
