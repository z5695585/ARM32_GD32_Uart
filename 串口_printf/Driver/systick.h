#ifndef _SYSTICK_DRV_H_
#define _SYSTICK_DRV_H_
#include "stdint.h"

/**
 * @description: Systick��ʼ��
 * @param  
 * @return:
 */
void SystickInit(void);

/**
 * @description: ��ȡϵͳ����ʱ��
 * @param  
 * @return:
 */
uint64_t GetSysRunTime(void);

#endif
