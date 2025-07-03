#ifndef _KEY_DRV_H_
#define _KEY_DRV_H_
#include "stdint.h"

#define KEY1_SHORT_PRESS 0x01
#define KEY1_LONG_PRESS	0x81
#define KEY2_SHORT_PRESS 0x02
#define KEY2_LONG_PRESS	0x82
#define KEY3_SHORT_PRESS 0x03
#define KEY3_LONG_PRESS	0x83

/**
***********************************************************
* @brief 按键初始化
* @param
* @return 
***********************************************************
*/
void KeyDrvInit(void);

/**
***********************************************************
* @brief 获取按键码值
* @param
* @return uint8_t，三个按键码值，短按0x01,0x02,0x03,
		  长按0x81,0x82,0x83;没有按下为0
***********************************************************
*/
uint8_t GetKeyVal(void);
#endif
