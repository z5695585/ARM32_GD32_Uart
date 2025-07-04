#include <stdint.h>
#include <stdio.h>
#include "gd32f30x.h"


#define WLEN 8		//定义UART字长

typedef struct
{
	uint32_t gpio_periph;			  // 哪个GPIO口
	rcu_periph_enum rcu_gpio_periph;  // 哪一个GPIO时钟
	uint32_t usart_periph;			  // 哪一个串口
	rcu_periph_enum rcu_usart_periph; // 哪一个串口时钟
	uint32_t tx_pin;				  // TX对应哪个GPIO管脚
	uint32_t rx_pin;				  // RX对应哪个GPIO管脚
	uint8_t nvic_irq;			  	  // 中断号
} UartHwInfo_t;

static UartHwInfo_t g_uartHwInfo = {GPIOA, RCU_GPIOA, USART0, RCU_USART0, GPIO_PIN_9, GPIO_PIN_10, USART0_IRQn};

static void Usb2ComGpioInit()
{
	/*使能GPIO时钟*/
	rcu_periph_clock_enable(g_uartHwInfo.rcu_gpio_periph);

	/*配置PA9，UART的TX*/
	gpio_init(g_uartHwInfo.gpio_periph, GPIO_MODE_AF_PP, GPIO_OSPEED_10MHZ, g_uartHwInfo.tx_pin);

	/*配置PA10，UART的RX*/
	gpio_init(g_uartHwInfo.gpio_periph, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_10MHZ, g_uartHwInfo.rx_pin);
}

static void Usb2ComUartInit(uint32_t baudRate)
{
	/*使能UART时钟*/
	rcu_periph_clock_enable(g_uartHwInfo.rcu_usart_periph);
	/*复位UART*/
	usart_deinit(g_uartHwInfo.usart_periph);
	/*通过USART_CTL0寄存器的WL设置字长*/
	usart_word_length_set(g_uartHwInfo.usart_periph, WLEN);
	/*通过USART_CTL0寄存器的PCEN设置校验位*/
	usart_parity_config(g_uartHwInfo.usart_periph, USART_PM_NONE);
	/*在USART_CTL1寄存器中写STB[1:0]位来设置停止位的长度*/
	usart_stop_bit_set(g_uartHwInfo.usart_periph, USART_STB_1BIT);
	/*在USART_BAUD寄存器中设置波特率*/
	usart_baudrate_set(g_uartHwInfo.usart_periph, baudRate);
	/*在USART_CTL0寄存器中设置TEN位，使能发送位*/
	usart_transmit_config(g_uartHwInfo.usart_periph, USART_TRANSMIT_ENABLE);
	/*使能接收*/
	usart_receive_config(g_uartHwInfo.usart_periph, USART_RECEIVE_ENABLE);
	/*使能USART中断*/
	usart_interrupt_enable(g_uartHwInfo.usart_periph, USART_INT_RBNE);
	/*使能总中断*/
	nvic_irq_enable(g_uartHwInfo.nvic_irq, 0, 0);
	/*在USART_CTL0寄存器中设置UEN位，使能UART*/
	usart_enable(g_uartHwInfo.usart_periph);
}

/**
 * @description: USB转串口用到的资源初始化
 * @param
 * @return:
 */
void Usb2ComInit(uint32_t baudRate)
{
	Usb2ComGpioInit();
	Usb2ComUartInit(baudRate);
}

static void (*pProcUartDataFunc)(uint8_t data);

/**
 * @description: 注册回调函数
 * @param
 * @return:
 */
void regUsb2ComCb(void (*pFunc)(uint8_t data))
{
	pProcUartDataFunc = pFunc;
}


/**
 * @description: UART中断服务函数
 * @param：
 * @return: 
 */
void USART0_IRQHandler(void)
{
	if(usart_interrupt_flag_get(g_uartHwInfo.usart_periph, USART_INT_FLAG_RBNE) != RESET)	//判断是否是RBNE进中断
	{
		usart_interrupt_flag_clear(g_uartHwInfo.usart_periph, USART_INT_FLAG_RBNE);			//清中断标志
		uint8_t uData = (uint8_t)usart_data_receive(g_uartHwInfo.usart_periph);				//读接收寄存器数据并暂存
		pProcUartDataFunc(uData);																//调函数处理数据，判断合法性并打包
	}
}

/**
 * @description: USB转串口发送测试
 * @param
 * @return:
 */
void Usb2ComTest(void)
{
	for (uint8_t i = 0; i < 255; i++)
	{
		// 发送数据
		usart_data_transmit(g_uartHwInfo.usart_periph, i);

		// 等待发送完成
		while (RESET == usart_flag_get(g_uartHwInfo.usart_periph, USART_FLAG_TBE))
			;
	}
}

/**
 * @description: printf打印默认输出到显示器，如果要输出到串口，必须重新实现fputc函数，将输出指向串口，称为重定向
 * @param
 * @return:
 */
int fputc(int ch, FILE *f)
{
	// 发送数据
	usart_data_transmit(g_uartHwInfo.usart_periph, (uint8_t)ch);

	// 等待发送完成
	while (RESET == usart_flag_get(g_uartHwInfo.usart_periph, USART_FLAG_TBE))
		;
	return ch;
}
