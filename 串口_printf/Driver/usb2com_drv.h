#ifndef _USB2COM_DRV_H_
#define _USB2COM_DRV_H_


/**
 * @description: USBת���ڷ��Ͳ���
 * @param 
 * @return:
 */
void Usb2ComTest(void);

/**
 * @description: USBת�����õ�����Դ��ʼ��
 * @param
 * @return:
 */
void Usb2ComInit(uint32_t baudRate);

/**
 * @description: ע��ص�����
 * @param
 * @return:
 */
void regUsb2ComCb(void (*pFunc)(uint8_t data));


#endif
