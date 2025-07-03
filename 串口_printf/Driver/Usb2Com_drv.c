#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "gd32f30x.h"
#include "led_drv.h"

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

#define DATA_LEN 20							//用于保存接收数据的数组的最大长度
#define FRAME_HEAD_0 0x55					//接收帧头第一个字节
#define FRAME_HEAD_1 0xAA					//接收帧头第二个字节
#define FRAME_DATA_LEN 3					//数据域长度
#define FRAME_LEN ((FRAME_DATA_LEN) + 4)	//帧总长度
#define FRAME_FUNC_INDEX 3					//数据帧表示命令的字节索引号
#define FRAME_FUNC_LED_CTRL 0x06			//命令号


/*
 *  定义一个结构体用于传递参数
 *  存储接收数据的数组中，第FRAME_FUNC_INDEX+1字节是表示LedNo,第FRAME_FUNC_INDEX+2字节表示LedState
 *  所以通过结构体指针的强制类型转换，使FRAME_FUNC_INDEX+1字节的地址指向这个数据结构
 *  然后就可以通过结构体成员顺序地访问这两个命令字
*/
typedef struct								
{
	uint8_t LedNo;
	uint8_t LedState;
} LedCtrlInfo_t;

static uint8_t g_dataIndex = 0;					//保存接收数据的索引号
static uint8_t g_rcvData[DATA_LEN] = {0};		//接收数据的数组
static bool g_packRcvd = false;					//接收完一包数据标志

/**
 * @description: UART接收到的数据处理，判断是否一个合法完整的数据帧
 * @param
 * @return:
 */
static void ProcUartData(uint8_t data)
{
	g_rcvData[g_dataIndex++] = data;
	switch(g_dataIndex)
	{
		case 1:                                                    	//如果第一个数据与帧头0不匹配，索引号清零并退出
			if(g_rcvData[g_dataIndex - 1] != FRAME_HEAD_0)
				g_dataIndex = 0;
				break;
		case 2:														//如果第一个数据与帧头0不匹配，索引号清零并退出
			if(g_rcvData[g_dataIndex - 1] != FRAME_HEAD_1)
				g_dataIndex = 0;
				break;
		case FRAME_LEN:												//索引号涨到FRAME_LEN后断帧，打包数据
			g_packRcvd = true;
			g_dataIndex = 0;
			break;
		default:
				break;
	}
		
}

/**
 * @description: 求异或和
 * @param：uint8_t *data  待求和的数组
           uint32_t len   要计算的数据长度（字节数）
 * @return: 一组数据的异或和
 */
static uint8_t CalXorSum(uint8_t *data, uint32_t len)
{
	uint8_t xorSum = 0;
	for(uint32_t i = 0; i < len; i++)
	{
		xorSum ^= data[i];
	}
	return xorSum;
}

/**
 * @description: LED控制
 * @param：LedCtrlInfo_t *ctrlData   
           结构体成员LedNo表示要控制的LED号，0-2
           结构体成员LedState表示要控制的LED状态，0表示关，非0表示开
 * @return: 
 */
static void CtrlLed(LedCtrlInfo_t *ctrlData )
{
	(ctrlData->LedState == 0) ? (TurnOffLed(ctrlData->LedNo)) : (TurnOnLed(ctrlData->LedNo));
}

/**
 * @description: 根据UART接收到的数据帧实施对应的逻辑
 * @param：
 * @return: 
 */
void Usb2ComTask(void)
{
	if(g_packRcvd == true)	//先判断是否有整包数据
	{
		g_packRcvd = false;
		if(CalXorSum(g_rcvData, FRAME_LEN-1) != g_rcvData[FRAME_LEN-1])	//判断数据包校验是否正确
		{
			return;
		}
		if(g_rcvData[FRAME_FUNC_INDEX] == FRAME_FUNC_LED_CTRL)	//根据命令号，调用函数实现控制
		{
			CtrlLed((LedCtrlInfo_t *)&g_rcvData[FRAME_FUNC_INDEX + 1]);
		}
	}
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
		ProcUartData(uData);																//调函数处理数据，判断合法性并打包
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
