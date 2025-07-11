#ifndef _QUEUE_H_
#define _QUEUE_H_
#include <stdint.h>


typedef struct
{
	uint32_t head;		//����ͷ���
	uint32_t tail;		//����β���
	uint32_t size;		//���д�С
	uint8_t *buffer;	//�洢�������ݵĻ����������ַ
} QueueType_t;

typedef enum
{
	QUEUE_OK = 0,		//OK
	QUEUE_ERROR,		//����
	QUEUE_OVERLOAD,		//������
	QUEUE_EMPTY			//���п�
} QueueStatus_t;

/**
 * @description: ���г�ʼ��
 * @param: 
 * @param
 * @return:
 */
void QueueInit(QueueType_t *rcvQueue, uint8_t *buffer, uint32_t size);

 /**
 * @description: �����������
 * @param
 * @return:
 */
QueueStatus_t EnQueue(QueueType_t *rcvQueue, uint8_t data);

/**
 * @description: �������ݳ���
 * @param
 * @return:
 */
QueueStatus_t DeQueue(QueueType_t *rcvQueue, uint8_t *pdata);

/**
 * @description: �������ݳ���
 * @param
 * @return:
 */
QueueStatus_t DeQueue(QueueType_t *rcvQueue, uint8_t *pdata);

/**
 * @description: һ�����ݳ���
 * @param
 * @return:
*/
uint32_t GroupDeQueue(QueueType_t *rcvQueue, uint8_t *bufferArray, uint32_t queueLen);

#endif
