#ifndef _RTOS_H
#define _RTOS_H

#include "task.h"
#include "stdio.h"

#define configMAX_TASK_NAME_LEN			16
#define configMAX_PRIORITIES		( 5 )
#define configMAX_SYSCALL_INTERRUPT_PRIORITY 	191 
typedef struct tskTaskControlBlock
{
	volatile StackType_t* pxTopOfStack;
	ListItem_t xStateListItem;
	StackType_t* pxStack;
	char pcTaskName[configMAX_TASK_NAME_LEN];
	TickType_t	xTicksToDelay;
	UBaseType_t	 uxPriority;
} tskTCB;
typedef tskTCB TCB_t;

TCB_t* volatile pxCurrentTCB = NULL;

#ifndef configSUPPORT_STATIC_ALLOCATION
#define configSUPPORT_STATIC_ALLOCATION    1
#endif

#ifdef _NVIC_PRIO_BITS
	#define configPRIO_BITS		_NVIC_PRIO_BITS
#else
	#define configPRIO_BITS		4
#endif

#define pdTASK_CODE                   TaskFunction_t

/*configPRIO_BITS��һ���궨�壬��������MCUʹ�ü�λ���ȼ�����ͬ��MCU�����в�ͬ�����ȼ�λ����
����STM32ʹ�õ���4λ�����configPRIO_BITSΪ412���������ȼ�������16����
������ȼ��Ǿ���15������Խ�����ȼ�Խ�ͣ����ȼ�Ϊ0~15��3��

configPRIO_BITS��Ӱ���������ж����ȼ���صĺ궨�壬����configKERNEL_INTERRUPT_PRIORITY��
configMAX_SYSCALL_INTERRUPT_PRIORITY�����Ƕ���Ҫ����configPRIO_BITS��ֵ�������ƻ����Ʋ�����
�Եõ���ȷ�����ȼ�ֵ45��*/

#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY		 0xf
#define configKERNEL_INTERRUPT_PRIORITY			( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )

#define xPortPendSVhandler		PendSV_Handler
#define xPortSysTickHandler		SysTick_Handler
#define vPortSVCHandler			SVC_Handler


#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY    5
/*configMAX_SYSCALL_INTERRUPT_PRIORITY��һ����FreeRTOSConfig.h�ж���ĺ꣬
���������ٽ��������ε��жϣ��ж����ȼ����ڸ�����ֵ���FreeRTOS�ܿأ�
�ж����ȼ����ڸ�����ֵ������1��������ֵ��Ҫ����Cortex-M�ں˵����ȼ��Ĵ�����λ���������ã�
Ҫ�������Чλ����2��FreeRTOSʹ��Cortex-M�ں˵�BASEPRI�Ĵ�����ʵ���ٽ�����
������RTOS�ں˿���ֻ����һ�����жϣ���˿����ṩһ�������ж�Ƕ��ģʽ*/
#define configMAX_SYSCALL_INTERRUPT_PRIORITY     ( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )

/*configASSERT��һ���꺯����������FreeRTOS�м��������ȷ�Ժ��ȶ��ԡ����Ĳ�����һ�����ʽ��
������ʽ��ֵΪ0����ʾ�����˴�����ô����ر��жϲ�����һ����ѭ����
�Ӷ��ó���ֹͣ�ڴ����λ�á��������Է��㿪����ʹ�õ���������λ���޸�����*/
#ifndef configASSERT
#define configASSERT( x )
#define configASSERT_DEFINED    0
#else
#define configASSERT_DEFINED    1
#endif

/*����������ȼ��ľ������������ַ����������� configUSE_PORT_OPTIMISED_TASK_SELECTION
�������ƣ�����Ϊ0ѡ��ͨ�÷���������Ϊ1ѡ����ݴ����� �Ż��ķ�����
�ú�Ĭ����portmacro.h�ж���Ϊ1����rtos��Ĭ��0*/
#ifndef configUSE_PORT_OPTIMISED_TASK_SELECTION
#define configUSE_PORT_OPTIMISED_TASK_SELECTION    0
#endif





#endif


















