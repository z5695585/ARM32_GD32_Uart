#include <stdint.h>
#include "gd32f30x.h"
#include "core_cm4.h"


/**
***********************************************************
* @brief DWT��ʼ������
* @param
* @return 
***********************************************************
*/
void DelayInit(void) 
{
	/* �ر� TRC */
	CoreDebug.DEMCR &= ~CoreDebug_DEMCR_TRCENA_Msk;
	/* �� TRC */
	CoreDebug.DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

	/* �رռ������� */
	DWT->CTRL &= ~DWT_CTRL_CYCCNTENA_Msk;
	/* �򿪼������� */
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

	/* �������� */
	DWT->CYCCNT = 0;
}

/**
***********************************************************
* @brief ΢�뼶��ʱ����
* @param nUs�������ʱʱ��( 2^32 / �ں���Ƶ ) * 10^6 us 
* @return 
***********************************************************
*/
void DelayNus(uint32_t nUs)
{
	uint32_t tickStart = DWT->CYCCNT;

	/* ת��ΪnUs��Ӧ��ʱ����������*/
	nUs *= (rcu_clock_freq_get(CK_AHB) / 1000000);

	/* ��ʱ�ȴ� */
	while ((DWT->CYCCNT - tickStart) < nUs);
}

/**
***********************************************************
* @brief ���뼶��ʱ����
* @param nMs����ʱʱ��n����
* @return 
***********************************************************
*/
void DelayNms(uint32_t nMs)
{
	for (uint32_t i = 0; i < nMs; i++)
	{
		DelayNus(1000);
	}
}
