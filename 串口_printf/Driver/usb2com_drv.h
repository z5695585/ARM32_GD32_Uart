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
 * @description: 注册回调函数
 * @param
 * @return:
 */
void regUsb2ComCb(void (*pFunc)(uint8_t data));


#endif
