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
* @brief ������ʼ��
* @param
* @return 
***********************************************************
*/
void KeyDrvInit(void);

/**
***********************************************************
* @brief ��ȡ������ֵ
* @param
* @return uint8_t������������ֵ���̰�0x01,0x02,0x03,
		  ����0x81,0x82,0x83;û�а���Ϊ0
***********************************************************
*/
uint8_t GetKeyVal(void);
#endif
