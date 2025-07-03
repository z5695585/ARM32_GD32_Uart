#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "gd32f30x.h"
#include "led_drv.h"

#define WLEN 8		//����UART�ֳ�

typedef struct
{
	uint32_t gpio_periph;			  // �ĸ�GPIO��
	rcu_periph_enum rcu_gpio_periph;  // ��һ��GPIOʱ��
	uint32_t usart_periph;			  // ��һ������
	rcu_periph_enum rcu_usart_periph; // ��һ������ʱ��
	uint32_t tx_pin;				  // TX��Ӧ�ĸ�GPIO�ܽ�
	uint32_t rx_pin;				  // RX��Ӧ�ĸ�GPIO�ܽ�
	uint8_t nvic_irq;			  	  // �жϺ�
} UartHwInfo_t;

static UartHwInfo_t g_uartHwInfo = {GPIOA, RCU_GPIOA, USART0, RCU_USART0, GPIO_PIN_9, GPIO_PIN_10, USART0_IRQn};

static void Usb2ComGpioInit()
{
	/*ʹ��GPIOʱ��*/
	rcu_periph_clock_enable(g_uartHwInfo.rcu_gpio_periph);

	/*����PA9��UART��TX*/
	gpio_init(g_uartHwInfo.gpio_periph, GPIO_MODE_AF_PP, GPIO_OSPEED_10MHZ, g_uartHwInfo.tx_pin);

	/*����PA10��UART��RX*/
	gpio_init(g_uartHwInfo.gpio_periph, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_10MHZ, g_uartHwInfo.rx_pin);
}

static void Usb2ComUartInit(uint32_t baudRate)
{
	/*ʹ��UARTʱ��*/
	rcu_periph_clock_enable(g_uartHwInfo.rcu_usart_periph);
	/*��λUART*/
	usart_deinit(g_uartHwInfo.usart_periph);
	/*ͨ��USART_CTL0�Ĵ�����WL�����ֳ�*/
	usart_word_length_set(g_uartHwInfo.usart_periph, WLEN);
	/*ͨ��USART_CTL0�Ĵ�����PCEN����У��λ*/
	usart_parity_config(g_uartHwInfo.usart_periph, USART_PM_NONE);
	/*��USART_CTL1�Ĵ�����дSTB[1:0]λ������ֹͣλ�ĳ���*/
	usart_stop_bit_set(g_uartHwInfo.usart_periph, USART_STB_1BIT);
	/*��USART_BAUD�Ĵ��������ò�����*/
	usart_baudrate_set(g_uartHwInfo.usart_periph, baudRate);
	/*��USART_CTL0�Ĵ���������TENλ��ʹ�ܷ���λ*/
	usart_transmit_config(g_uartHwInfo.usart_periph, USART_TRANSMIT_ENABLE);
	/*ʹ�ܽ���*/
	usart_receive_config(g_uartHwInfo.usart_periph, USART_RECEIVE_ENABLE);
	/*ʹ��USART�ж�*/
	usart_interrupt_enable(g_uartHwInfo.usart_periph, USART_INT_RBNE);
	/*ʹ�����ж�*/
	nvic_irq_enable(g_uartHwInfo.nvic_irq, 0, 0);
	/*��USART_CTL0�Ĵ���������UENλ��ʹ��UART*/
	usart_enable(g_uartHwInfo.usart_periph);
}

/**
 * @description: USBת�����õ�����Դ��ʼ��
 * @param
 * @return:
 */
void Usb2ComInit(uint32_t baudRate)
{
	Usb2ComGpioInit();
	Usb2ComUartInit(baudRate);
}

/**
 * @description: USBת���ڷ��Ͳ���
 * @param
 * @return:
 */
void Usb2ComTest(void)
{
	for (uint8_t i = 0; i < 255; i++)
	{
		// ��������
		usart_data_transmit(g_uartHwInfo.usart_periph, i);

		// �ȴ��������
		while (RESET == usart_flag_get(g_uartHwInfo.usart_periph, USART_FLAG_TBE))
			;
	}
}

#define DATA_LEN 20							//���ڱ���������ݵ��������󳤶�
#define FRAME_HEAD_0 0x55					//����֡ͷ��һ���ֽ�
#define FRAME_HEAD_1 0xAA					//����֡ͷ�ڶ����ֽ�
#define FRAME_DATA_LEN 3					//�����򳤶�
#define FRAME_LEN ((FRAME_DATA_LEN) + 4)	//֡�ܳ���
#define FRAME_FUNC_INDEX 3					//����֡��ʾ������ֽ�������
#define FRAME_FUNC_LED_CTRL 0x06			//�����


/*
 *  ����һ���ṹ�����ڴ��ݲ���
 *  �洢�������ݵ������У���FRAME_FUNC_INDEX+1�ֽ��Ǳ�ʾLedNo,��FRAME_FUNC_INDEX+2�ֽڱ�ʾLedState
 *  ����ͨ���ṹ��ָ���ǿ������ת����ʹFRAME_FUNC_INDEX+1�ֽڵĵ�ַָ��������ݽṹ
 *  Ȼ��Ϳ���ͨ���ṹ���Ա˳��ط���������������
*/
typedef struct								
{
	uint8_t LedNo;
	uint8_t LedState;
} LedCtrlInfo_t;

static uint8_t g_dataIndex = 0;					//����������ݵ�������
static uint8_t g_rcvData[DATA_LEN] = {0};		//�������ݵ�����
static bool g_packRcvd = false;					//������һ�����ݱ�־

/**
 * @description: UART���յ������ݴ����ж��Ƿ�һ���Ϸ�����������֡
 * @param
 * @return:
 */
static void ProcUartData(uint8_t data)
{
	g_rcvData[g_dataIndex++] = data;
	switch(g_dataIndex)
	{
		case 1:                                                    	//�����һ��������֡ͷ0��ƥ�䣬���������㲢�˳�
			if(g_rcvData[g_dataIndex - 1] != FRAME_HEAD_0)
				g_dataIndex = 0;
				break;
		case 2:														//�����һ��������֡ͷ0��ƥ�䣬���������㲢�˳�
			if(g_rcvData[g_dataIndex - 1] != FRAME_HEAD_1)
				g_dataIndex = 0;
				break;
		case FRAME_LEN:												//�������ǵ�FRAME_LEN���֡���������
			g_packRcvd = true;
			g_dataIndex = 0;
			break;
		default:
				break;
	}
		
}

/**
 * @description: ������
 * @param��uint8_t *data  ����͵�����
           uint32_t len   Ҫ��������ݳ��ȣ��ֽ�����
 * @return: һ�����ݵ�����
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
 * @description: LED����
 * @param��LedCtrlInfo_t *ctrlData   
           �ṹ���ԱLedNo��ʾҪ���Ƶ�LED�ţ�0-2
           �ṹ���ԱLedState��ʾҪ���Ƶ�LED״̬��0��ʾ�أ���0��ʾ��
 * @return: 
 */
static void CtrlLed(LedCtrlInfo_t *ctrlData )
{
	(ctrlData->LedState == 0) ? (TurnOffLed(ctrlData->LedNo)) : (TurnOnLed(ctrlData->LedNo));
}

/**
 * @description: ����UART���յ�������֡ʵʩ��Ӧ���߼�
 * @param��
 * @return: 
 */
void Usb2ComTask(void)
{
	if(g_packRcvd == true)	//���ж��Ƿ�����������
	{
		g_packRcvd = false;
		if(CalXorSum(g_rcvData, FRAME_LEN-1) != g_rcvData[FRAME_LEN-1])	//�ж����ݰ�У���Ƿ���ȷ
		{
			return;
		}
		if(g_rcvData[FRAME_FUNC_INDEX] == FRAME_FUNC_LED_CTRL)	//��������ţ����ú���ʵ�ֿ���
		{
			CtrlLed((LedCtrlInfo_t *)&g_rcvData[FRAME_FUNC_INDEX + 1]);
		}
	}
}

/**
 * @description: UART�жϷ�����
 * @param��
 * @return: 
 */
void USART0_IRQHandler(void)
{
	if(usart_interrupt_flag_get(g_uartHwInfo.usart_periph, USART_INT_FLAG_RBNE) != RESET)	//�ж��Ƿ���RBNE���ж�
	{
		usart_interrupt_flag_clear(g_uartHwInfo.usart_periph, USART_INT_FLAG_RBNE);			//���жϱ�־
		uint8_t uData = (uint8_t)usart_data_receive(g_uartHwInfo.usart_periph);				//�����ռĴ������ݲ��ݴ�
		ProcUartData(uData);																//�������������ݣ��жϺϷ��Բ����
	}
}


/**
 * @description: printf��ӡĬ���������ʾ�������Ҫ��������ڣ���������ʵ��fputc�����������ָ�򴮿ڣ���Ϊ�ض���
 * @param
 * @return:
 */
int fputc(int ch, FILE *f)
{
	// ��������
	usart_data_transmit(g_uartHwInfo.usart_periph, (uint8_t)ch);

	// �ȴ��������
	while (RESET == usart_flag_get(g_uartHwInfo.usart_periph, USART_FLAG_TBE))
		;
	return ch;
}
