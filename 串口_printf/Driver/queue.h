#ifndef _QUEUE_H_
#define _QUEUE_H_
#include <stdint.h>


typedef struct
{
	uint32_t head;		//队列头序号
	uint32_t tail;		//队列尾序号
	uint32_t size;		//队列大小
	uint8_t *buffer;	//存储队列数据的缓冲区数组地址
} QueueType_t;

typedef enum
{
	QUEUE_OK = 0,		//OK
	QUEUE_ERROR,		//错误
	QUEUE_OVERLOAD,		//队列满
	QUEUE_EMPTY			//队列空
} QueueStatus_t;

/**
 * @description: 队列初始化
 * @param: 
 * @param
 * @return:
 */
void QueueInit(QueueType_t *rcvQueue, uint8_t *buffer, uint32_t size);

 /**
 * @description: 单个数据入队
 * @param
 * @return:
 */
QueueStatus_t EnQueue(QueueType_t *rcvQueue, uint8_t data);

/**
 * @description: 单个数据出队
 * @param
 * @return:
 */
QueueStatus_t DeQueue(QueueType_t *rcvQueue, uint8_t *pdata);

/**
 * @description: 单个数据出队
 * @param
 * @return:
 */
QueueStatus_t DeQueue(QueueType_t *rcvQueue, uint8_t *pdata);

/**
 * @description: 一组数据出队
 * @param
 * @return:
*/
uint32_t GroupDeQueue(QueueType_t *rcvQueue, uint8_t *bufferArray, uint32_t queueLen);

#endif
