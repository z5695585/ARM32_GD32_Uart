#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "gd32f30x.h"
#include "usb2com_drv.h"
#include "led_drv.h"


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
 * @description: APP初始化,调用下层接口函数，注册回调函数
 * @param
 * @return:
 */
void Usb2ComAppInit(void)
{
	regUsb2ComCb(ProcUartData);
}
