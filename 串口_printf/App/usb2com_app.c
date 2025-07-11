#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "gd32f30x.h"
#include "usb2com_drv.h"
#include "led_drv.h"
#include "queue.h"

/**
******************************************************************************************
����ʽ	֡ͷ0    ֡ͷ1    ���ݳ���    ������    LED���    ��/��    ���У������
		0x55	 0xAA	   0x03 	   0x06      0x00       0x01       0xXX
******************************************************************************************
*/
#define FRAME_HEAD_0 0x55					//����֡ͷ��һ���ֽ�
#define FRAME_HEAD_1 0xAA					//����֡ͷ�ڶ����ֽ�
#define FRAME_LEN_MIN 7						//��С֡����
#define FRAME_LEN_MAX 11					//���֡����
#define FRAME_DATA_LEN_MAX (FRAME_LEN_MAX - 4)
#define FRAME_FUNC_INDEX 3					//����֡��ʾ������ֽ�������
#define FRAME_FUNC_LED_CTRL 0x06			//������

#define DATA_LEN 77								//���ڱ���������ݵ��������󳤶�
static uint8_t g_rcvData[DATA_LEN] = {0};		//�������ݵ�����
static QueueType_t g_rcvQueue = {0};


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

/**
 * @description: UART���յ������ݴ����ж��Ƿ�һ���Ϸ�����������֡
 * @param
 * @return:
 */
static void ProcUartData(uint8_t data)
{
	EnQueue(&g_rcvQueue, data);
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
	uint8_t readBuf[DATA_LEN] = {0};
	while(DeQueue(&g_rcvQueue, &readBuf[0]) == QUEUE_OK)
	{
		if(readBuf[0] != FRAME_HEAD_0)
		{
			continue;
		}
		if((DeQueue(&g_rcvQueue, &readBuf[1]) == QUEUE_EMPTY) || (readBuf[1] != FRAME_HEAD_1))
		{
			continue;
		}
		if(DeQueue(&g_rcvQueue, &readBuf[2]) == QUEUE_EMPTY || readBuf[2] > FRAME_DATA_LEN_MAX)
		{
			continue;
		}
		if(GroupDeQueue(&g_rcvQueue, &readBuf[3], (readBuf[2]+1)) != readBuf[2]+1)
		{
			continue;
		}
		if(CalXorSum(readBuf, (readBuf[2] + 3)) != readBuf[readBuf[2] + 3])	//�ж����ݰ�У���Ƿ���ȷ
		{
			continue;
		}
		if(readBuf[FRAME_FUNC_INDEX] == FRAME_FUNC_LED_CTRL)	//��������ţ����ú���ʵ�ֿ���
		{
			CtrlLed((LedCtrlInfo_t *)&readBuf[FRAME_FUNC_INDEX + 1]);
		}
		for(uint32_t i = 0; i < readBuf[2] + 4; i++ )
		{
			printf("%02x", readBuf[i]);
		}
		printf("\n");
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
	QueueInit(&g_rcvQueue, g_rcvData, DATA_LEN);
}
