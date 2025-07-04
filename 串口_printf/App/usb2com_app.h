#ifndef _USB2COM_APP_H_
#define _USB2COM_APP_H_

/**
 * @description: APP初始化,调用下层接口函数，注册回调函数
 * @param
 * @return:
 */
void Usb2ComAppInit(void);

/**
 * @description: 根据UART接收到的数据帧实施对应的逻辑
 * @param：
 * @return: 
 */
void Usb2ComTask(void);



#endif
