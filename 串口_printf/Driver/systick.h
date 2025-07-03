#ifndef _SYSTICK_DRV_H_
#define _SYSTICK_DRV_H_
#include "stdint.h"

/**
 * @description: Systick初始化
 * @param  
 * @return:
 */
void SystickInit(void);

/**
 * @description: 获取系统运行时间
 * @param  
 * @return:
 */
uint64_t GetSysRunTime(void);

#endif
