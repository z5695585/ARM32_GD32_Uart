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
 * @description: ����UART���յ�������֡ʵʩ��Ӧ���߼�
 * @param��
 * @return: 
 */
void Usb2ComTask(void);


#endif
