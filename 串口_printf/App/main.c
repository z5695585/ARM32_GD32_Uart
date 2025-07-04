#include <stdint.h>
#include <stdio.h>
#include "gd32f30x.h"
#include "led_drv.h"
#include "usb2com_drv.h"
#include "usb2com_app.h"

static void DrvInit(void)
{
	LedInit();
	Usb2ComInit(115200);
}

static void AppInit(void)
{
	Usb2ComAppInit();
}


int main(void)
{	
	DrvInit();
	AppInit();
	printf("This is a test.");
	while(1)
	{
		Usb2ComTask();
	}
}
