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
/*_forceinline是一个编译器指令，用于告诉编译器强制将一个函数内联到调用它的地方，
以提高程序的执行效率1。在FreeRTOS中，__forceinline主要用于一些简单的内核函数，如任务切换，
事件标志，软件定时器等2。使用__forceinline可以减少函数调用的开销，提高实时性，
但也可能增加代码的大小*/
/*内联是一种编程技术，它可以让编译器用函数的代码替换掉函数的调用，从而减少函数调用的开销，
提高程序的运行效率1。内联函数通常用 inline 关键字来声明，或者在类中直接定义2。但是，
内联函数并不保证一定会被编译器内联，编译器会根据函数的复杂度，调用频率，
代码大小等因素来决定是否进行内联*/
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

/*查找最高优先级的就绪任务有两种方法，具体由 configUSE_PORT_OPTIMISED_TASK_SELECTION
这个宏控制，定义为0选择通用方法，定义为1选择根据处理器 优化的方法，
该宏默认在portmacro.h中定义为1，在rtos中默认0*/
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