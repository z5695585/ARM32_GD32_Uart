#include <stdint.h>
#include "gd32f30x.h"
#include "led_drv.h"
#include "key_drv.h"

void delay(uint32_t i)
{
	while(i--);
}

int main(void)
{
	LedInit();
	KeyDrvInit();	
	while(1)
	{		
		ToggleLed(LED2);
		delay(9999999);
		
		
	}
	
	
	
}

