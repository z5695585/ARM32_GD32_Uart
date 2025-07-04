#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "gd32f30x.h"
#include "usb2com_drv.h"
#include "led_drv.h"


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
 * @description: APP��ʼ��,�����²�ӿں�����ע��ص�����
 * @param
 * @return:
 */
void Usb2ComAppInit(void)
{
	regUsb2ComCb(ProcUartData);
}
