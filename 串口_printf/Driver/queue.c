#include <stdint.h>
#include <stdio.h>
#include <queue.h>

#define MAX_BUF_SIZE 20

uint8_t g_rcvDataBuf[MAX_BUF_SIZE];


/**
 * @description: ���г�ʼ��������ͷ��β������㡣ͬʱָ��һ������������ݵ�����
 * @param: *rcvQueue��Ҫ��ʼ���Ķ��б���ָ��
 * @param: *buffer��������еĻ���������ָ��
 * @param: size�����д�С
 * @return:
 */
void QueueInit(QueueType_t *rcvQueue, uint8_t *buffer, uint32_t size)
 {
	rcvQueue->head = 0;
	rcvQueue->tail = 0;
	rcvQueue->buffer = buffer;
	rcvQueue->size = size;
 }
 
 
 /**
 * @description: ����������ӣ�����Ҫ��ӵ����ݱ���Ķ���ָ���Ļ��������в�����λ�����
 * @param: *rcvQueue��Ŀ����б���ָ��
 * @param: data��Ҫ��ӵ�����
 * @return: ��ӳɹ�����״̬
 */
QueueStatus_t EnQueue(QueueType_t *rcvQueue, uint8_t data)
{
	if (rcvQueue == NULL || rcvQueue->buffer == NULL) 
	{
		return QUEUE_ERROR; 
	}
	if((rcvQueue->tail + 1) % rcvQueue->size == rcvQueue->head)
	{
		return	QUEUE_OVERLOAD;
	}
	rcvQueue->buffer[rcvQueue->tail] = data;
	rcvQueue->tail = (rcvQueue->tail + 1) % rcvQueue->size;
	return QUEUE_OK;	
}
 
 
 
 /**
 * @description: �������ݳ���
 * @param
 * @return:
 */
QueueStatus_t DeQueue(QueueType_t *rcvQueue, uint8_t *pdata)
{
	if (rcvQueue == NULL || pdata == NULL || rcvQueue->buffer == NULL) 
	{
		return QUEUE_ERROR; 
	}
	if(rcvQueue->head == rcvQueue->tail)
	{
		return QUEUE_EMPTY;
	}		
	*pdata = rcvQueue->buffer[rcvQueue->head];
	rcvQueue->head = (rcvQueue->head + 1) % rcvQueue->size;
	return QUEUE_OK;	
}
 
/**
 * @description: һ���������
 * @param
 * @return:
*/
uint32_t GroupEnQueue(QueueType_t *rcvQueue, uint8_t *array, uint32_t arraySize)
{
	uint32_t i;
	if (rcvQueue == NULL || array == NULL) 
	{
		return 0;
	}
	for(i = 0; i < arraySize; i++)
	{
		if(EnQueue(rcvQueue, array[i]) != QUEUE_OK)
		{
			return i;
		}
	}
	return arraySize;
}

 
/**
 * @description: һ�����ݳ���
 * @param
 * @return:
*/

uint32_t GroupDeQueue(QueueType_t *rcvQueue, uint8_t *bufferArray, uint32_t queueLen)
{
	uint32_t i;
	if (rcvQueue == NULL || bufferArray == NULL) 
	{
		return 0;
	}
	for(i = 0; i < queueLen; i++)
	{
		if(DeQueue(rcvQueue, &bufferArray[i]) != QUEUE_OK)
		{
			return i;
		}
	}
	return queueLen;
}
