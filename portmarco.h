#ifndef PORTMACRO_H
#define PORTMACRO_H



#include "rtos.h"

#define portCHAR	char
#define portFLOAT	float
#define portDOUBLE	double
#define portLONG	long
#define portSHORT	short
#define portSTACK_TYPE	uint32_t
#define portBASE_TYPE	long

typedef portSTACK_TYPE		StackType_t;
typedef long BaseType_t;
typedef unsigned long UBaseType_t;

#define portDISABLE_INTERRUPTS( ) vPortRaiseBASEPRI()


#define portMAX_DELAY              ( TickType_t ) 0xffffffffUL
void vPortRaiseBASEPRI(void)
{
	unsigned long ulReturn;
	uint32_t ulNewBASEPRI = configMAX_SYSCALL_INTERRUPT_PRIORITY;
	__asm
	{
		mrs ulReturn, basepri
		msr basepri, ulNewBASEPRI
		dsb
		isb
	}
	return ulReturn;
}


#define portENABLE_INTERRUPTS() vPortSetBASEPRI( 0 )

#define portCLEAR_INTERRUPT_MASK_FROM_ISR(x) vPortSetBASEPRI(x)

void vPortSetBASEPRI(uint32_t ulBASEPRI)
{
	__asm
	{
		msr basepri, ulBASEPRI
	}
}

#define vPortClearBASEPRIFromISR() vPortSetBASEPRI(0)

#define portENTER_CRITICAL()		vPortEnterCritical()

#define portDISABLE_INTERRUPT()			vPortRaiseBaiseBASEPRI()

#ifndef portFORCE_INLINE
/*_forceinline��һ��������ָ����ڸ��߱�����ǿ�ƽ�һ�������������������ĵط���
����߳����ִ��Ч��1����FreeRTOS�У�__forceinline��Ҫ����һЩ�򵥵��ں˺������������л���
�¼���־�������ʱ����2��ʹ��__forceinline���Լ��ٺ������õĿ��������ʵʱ�ԣ�
��Ҳ�������Ӵ���Ĵ�С*/
/*������һ�ֱ�̼������������ñ������ú����Ĵ����滻�������ĵ��ã��Ӷ����ٺ������õĿ�����
��߳��������Ч��1����������ͨ���� inline �ؼ���������������������ֱ�Ӷ���2�����ǣ�
��������������֤һ���ᱻ����������������������ݺ����ĸ��Ӷȣ�����Ƶ�ʣ�
�����С�������������Ƿ��������*/
#define portFORCE_INLINE    __forceinline
#endif

static portFORCE_INLINE void vPortRaiseBASEPRI(void)
{
	uint32_t ulNewBASERPRI = configMAX_SYSCALL_INTERRUPT_PRIORITY;

	__asm
	{
		msr basepri, ulNewBASERPRI;
		dsb
		isb
	}
}

#define portSET_INTERRUPT_MASK_FROM_ISR()		ulPortRaiseBASEPRI();
static portFORCE_INLINE uint32_t ulPortRaiseBASEPRI(void)
{
	uint32_t ulReturn, ulNewBASEPRI = configMAX_SYSCALL_INTERRUPT_PRIORITY;
	__asm
	{
		mrs ulReturn, basepri
		msr basepri, ulNewBASEPRI
		dsb
		isb
	}
	return ulReturn;
}

#define portEXIT_CRITICAL()		vPortExitCritical()

static portFORCE_INLINE void vPortSetBASEPRI(uint32_t ulBASEPRI)
{
	__asm
	{
		msr basepri,ulBASEPRI
	}
}

/*����������ȼ��ľ������������ַ����������� configUSE_PORT_OPTIMISED_TASK_SELECTION
�������ƣ�����Ϊ0ѡ��ͨ�÷���������Ϊ1ѡ����ݴ����� �Ż��ķ�����
�ú�Ĭ����portmacro.h�ж���Ϊ1����rtos��Ĭ��0*/
#ifndef configUSE_PORT_OPTIMISED_TASK_SELECTION
#define configUSE_PORT_OPTIMISED_TASK_SELECTION    1
#endif


#define portRECORD_READY_PRIORITY( uxPriority,uxReadyPriorities )\
		( uxReadyPriorities) |= (1UL << (uxPriority))
#define portRESET_READY_PRIORITY(uxPriority,uxReadyPriorities )\
		( uxReadyPriorities) &=~( 1UL << (uxPriority))

#define portGET_HIGHEST_PRIORITY( uxTopPriority, uxReadyPriorities)\
		uxTopPriority =(31UL- (uint32_t) __clz(( uxReadyPriorities )))








#endif