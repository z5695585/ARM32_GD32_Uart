#include <stdint.h>
#include "gd32f30x.h"
#include "systick.h"
#include "led_drv.h"

#define KEY_NUM_MAX (sizeof(g_keyList) / sizeof(g_keyList[0]))
#define CONFIRM_TIME 30
#define LONGPRESS_TIME 1000

typedef enum
{
	KEY_RELEASE = 0,
	KEY_CONFIRM,
	KEY_SHORTPRESS,
	KEY_LONGPRESS,
} KEY_STATE;

typedef struct 
{
    rcu_periph_enum rcu_periph;
    uint32_t gpio_periph;
    uint32_t pin;
} keyPara;

typedef struct
{
	KEY_STATE keyState;
	uint64_t prvSysTime;
} Key_Info_t;


static keyPara g_keyList[] = {
    {RCU_GPIOA, GPIOA, GPIO_PIN_0},
	{RCU_GPIOG, GPIOG, GPIO_PIN_13},
	{RCU_GPIOG, GPIOG, GPIO_PIN_14},
	{RCU_GPIOG, GPIOG, GPIO_PIN_15}
};




static Key_Info_t s_keyInfo[KEY_NUM_MAX];

/**
***********************************************************
* @brief 按键初始化
* @param
* @return 
***********************************************************
*/
void KeyDrvInit(void)
{
	/*初始化按键的GPIO*/
	for (uint8_t i = 0; i < KEY_NUM_MAX; i++)
    {
        rcu_periph_clock_enable(g_keyList[i].rcu_periph);
		gpio_init(g_keyList[i].gpio_periph, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_2MHZ, g_keyList[i].pin);
    }	
}

/**
***********************************************************
* @brief 按键扫描，static函数，就在本文件内调用，用于给GetKeyVal获得按键码值
* @param
* @return uint8_t，三个按键码值，短按0x01,0x02,0x03,
		  长按0x81,0x82,0x83;没有按下为0
***********************************************************
*/
static uint8_t KeyScan(uint8_t keyIndex)
{
	uint8_t keyPress;
	uint64_t curSysTime;
	
	keyPress = gpio_input_bit_get(g_keyList[keyIndex].gpio_periph, g_keyList[keyIndex].pin);	
	
	switch (s_keyInfo[keyIndex].keyState)
	{
	case KEY_RELEASE:
		if(!keyPress)
		{
			s_keyInfo[keyIndex].keyState = KEY_CONFIRM;
			s_keyInfo[keyIndex].prvSysTime = GetSysRunTime();
		}								

		break;
		
	case KEY_CONFIRM:
		if (!keyPress)
		{
			curSysTime = GetSysRunTime();
			if (curSysTime - s_keyInfo[keyIndex].prvSysTime > CONFIRM_TIME)
			{
				s_keyInfo[keyIndex].keyState = KEY_SHORTPRESS;
			}			
		}
		else
		{
			s_keyInfo[keyIndex].keyState = KEY_RELEASE;
		}
		break;
		
	case KEY_SHORTPRESS:	
		if (keyPress)
		{
			s_keyInfo[keyIndex].keyState = KEY_RELEASE;
			return(keyIndex + 1);
		}
		else
		{
			curSysTime = GetSysRunTime();
			if (curSysTime - s_keyInfo[keyIndex].prvSysTime > LONGPRESS_TIME)
			{
				s_keyInfo[keyIndex].keyState = KEY_LONGPRESS;
			}			
		}
		break;
		
	case KEY_LONGPRESS:
		if (keyPress)
		{
			s_keyInfo[keyIndex].keyState = KEY_RELEASE;
			return(keyIndex + 0x81);
		}
		break;
		
	default:
//		s_keyInfo[keyIndex].keyState = KEY_RELEASE;
		break;
	}
	return 0;
}

/**
***********************************************************
* @brief 获取按键码值
* @param
* @return uint8_t，三个按键码值，短按0x01,0x02,0x03,
		  长按0x81,0x82,0x83;没有按下为0
***********************************************************
*/
uint8_t GetKeyVal(void)
{
	uint8_t temp = 0;
	for (uint8_t i = 0; i < KEY_NUM_MAX; i++)
    {
        temp = KeyScan(i);
		if (temp != 0)
		{
			break;
		}
    }
	return temp;
}
