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

/*configPRIO_BITS是一个宏定义，用来设置MCU使用几位优先级，不同的MCU可能有不同的优先级位数，
例如STM32使用的是4位，因此configPRIO_BITS为412。这样优先级数就是16个，
最低优先级那就是15（数字越大优先级越低，优先级为0~15）3。

configPRIO_BITS会影响其他的中断优先级相关的宏定义，例如configKERNEL_INTERRUPT_PRIORITY和
configMAX_SYSCALL_INTERRUPT_PRIORITY，它们都需要根据configPRIO_BITS的值进行左移或右移操作，
以得到正确的优先级值45。*/

#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY		 0xf
#define configKERNEL_INTERRUPT_PRIORITY			( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )

#define xPortPendSVhandler		PendSV_Handler
#define xPortSysTickHandler		SysTick_Handler
#define vPortSVCHandler			SVC_Handler


#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY    5
/*configMAX_SYSCALL_INTERRUPT_PRIORITY是一个在FreeRTOSConfig.h中定义的宏，
用于区分临界区可屏蔽的中断，中断优先级低于该设置值则归FreeRTOS管控，
中断优先级高于该设置值则不受限1。这个宏的值需要根据Cortex-M内核的优先级寄存器的位数进行设置，
要以最高有效位对齐2。FreeRTOS使用Cortex-M内核的BASEPRI寄存器来实现临界区，
这允许RTOS内核可以只屏蔽一部分中断，因此可以提供一个灵活的中断嵌套模式*/
#define configMAX_SYSCALL_INTERRUPT_PRIORITY     ( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )

/*configASSERT是一个宏函数，用于在FreeRTOS中检查程序的正确性和稳定性。它的参数是一个表达式，
如果表达式的值为0，表示出现了错误，那么它会关闭中断并进入一个死循环，
从而让程序停止在错误的位置。这样可以方便开发者使用调试器来定位和修复错误*/
#ifndef configASSERT
#define configASSERT( x )
#define configASSERT_DEFINED    0
#else
#define configASSERT_DEFINED    1
#endif

/*查找最高优先级的就绪任务有两种方法，具体由 configUSE_PORT_OPTIMISED_TASK_SELECTION
这个宏控制，定义为0选择通用方法，定义为1选择根据处理器 优化的方法，
该宏默认在portmacro.h中定义为1，在rtos中默认0*/
#ifndef configUSE_PORT_OPTIMISED_TASK_SELECTION
#define configUSE_PORT_OPTIMISED_TASK_SELECTION    0
#endif





#endif


















