#include <stdint.h>
#include <stdio.h>
#include "gd32f30x.h"
#include "led_drv.h"
#include "usb2com_drv.h"



int main(void)
{
	LedInit();
	Usb2ComInit(115200);
	printf("This is a test.");
	while(1)
	{
		Usb2ComTask();
	}
}
