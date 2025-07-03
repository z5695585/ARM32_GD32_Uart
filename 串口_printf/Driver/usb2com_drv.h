#ifndef _USB2COM_DRV_H_
#define _USB2COM_DRV_H_


/**
 * @description: USB转串口发送测试
 * @param 
 * @return:
 */
void Usb2ComTest(void);

/**
 * @description: USB转串口用到的资源初始化
 * @param
 * @return:
 */
void Usb2ComInit(uint32_t baudRate);

/**
 * @description: 根据UART接收到的数据帧实施对应的逻辑
 * @param：
 * @return: 
 */
void Usb2ComTask(void);


#endif
