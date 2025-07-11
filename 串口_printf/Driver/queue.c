#include <stdint.h>
#include <stdio.h>
#include <queue.h>

#define MAX_BUF_SIZE 20

uint8_t g_rcvDataBuf[MAX_BUF_SIZE];


/**
 * @description: 队列初始化，队列头、尾序号清零。同时指定一个保存队列数据的数组
 * @param: *rcvQueue，要初始化的队列变量指针
 * @param: *buffer，保存队列的缓冲区数组指针
 * @param: size，队列大小
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
 * @description: 单个数据入队，把需要入队的数据保存的队列指定的缓冲数组中并更新位置序号
 * @param: *rcvQueue，目标队列变量指针
 * @param: data，要入队的数据
 * @return: 入队成功与否的状态
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
 * @description: 单个数据出队
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
 * @description: 一组数据入队
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
 * @description: 一组数据出队
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
