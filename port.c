#include "port.h"
#include "task.h"
#include "rtos.h"
#include "projdefs.h"
#include "portmarco.h"
#include "list.h"

/*configKERNEL_INTERRUPT_PRIORITY是一个宏定义，用来设置FreeRTOS内核自己的中断优先级。
因为FreeRTOS内核中断不允许抢占用户使用的中断，因此这个宏一般定义为硬件最低优先级12。

在ARM Cortex-M内核中，数字上的低优先级值被用来指定逻辑上的高中断优先级。例如，
一个被分配数字优先级为2的中断的逻辑优先级高于一个被分配数字优先级为5的中断的逻辑优先级。
因此，configKERNEL_INTERRUPT_PRIORITY的值应该是数字上的最大值，例如2553。

在FreeRTOSConfig.h文件中，可以看到configKERNEL_INTERRUPT_PRIORITY的定义如下：

#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY 0xf
#define configKERNEL_INTERRUPT_PRIORITY ( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )

其中，configLIBRARY_LOWEST_INTERRUPT_PRIORITY是硬件最低优先级的值，
configPRIO_BITS是可用的优先级位的数量，这取决于具体的微控制器4。通过左移操作，
将configLIBRARY_LOWEST_INTERRUPT_PRIORITY的值转换为ARM Cortex-M内核本身想要的优先级值，
即已经移动到字节的最高有效位3。*/

static void* prvTaskExitError(void)
{
	for (;;);
}




/*portNVIC_PENDSV_PRI是用来设置PendSV的中断优先级的，它将configKERNEL_INTERRUPT_PRIORITY
左移16位，然后赋给NVIC_SYSPRI3寄存器的bit23-bit16，这样就可以控制PendSV的优先级。
PendSV是一个可挂起的系统调用，它在FreeRTOS中用来进行任务切换。

portNVIC_SYSTICK_PRI是用来设置SysTick的中断优先级的，
它将configKERNEL_INTERRUPT_PRIORITY左移24位，
然后赋给NVIC_SYSPRI3寄存器的bit31-bit24，这样就可以控制SysTick的优先级。
SysTick是一个系统滴答定时器，它在FreeRTOS中用来触发任务切换或延时。*/

/*PendSV和SysTick是Cortex-M内核中的两种异常，它们在FreeRTOS中用来实现任务调度和切换。

PendSV（可挂起的系统调用），异常编号为14，可编程。它是用来进行任务切换的，
当有任务的状态发生变化时，就会触发PendSV异常，通过改变进程栈指针（PSP）切换到不同的任务。
PendSV异常的优先级是最低的，它会在所有其他的中断处理完成后才执行，
这样可以保证操作系统的实时性。

SysTick（系统滴答定时器），异常编号为15，可编程。它是用来作为操作系统的心跳时钟的，
一般默认为1ms，每次中断时，系统会检测是否有新的就绪任务需要运行，如果有，
就会悬挂PendSV异常，以便缓期执行上下文切换15。SysTick的优先级一般设置为最低，
以免打断用户的外部中断26。
PendSV 和 SysTick 是 ARM Cortex-M 内核中的两种异常，它们在 FreeRTOS 中用来实现任务调度和切换。
它们的中断优先级设置是操作 0xE000_ED20 地址的，这个地址是 NVIC_SYSPRI3 寄存器的地址，
它用来存储 PendSV、SysTick 和 Debug Monitor 的优先级34。

NVIC_SYSPRI3 寄存器是一个 32 位的数据，其中 bit31-bit24 存储 SysTick 的优先级，
bit23-bit16 存储 PendSV 的优先级，bit15-bit8 存储 Debug Monitor 的优先级，bit7-bit0 保留34。
因此，要设置 PendSV 和 SysTick 的优先级，就需要将优先级值左移到相应的位置，然后写入到 NVIC_SYSPRI3 寄存器中。

例如，如果要将 PendSV 的优先级设置为最低（0xf），就需要将 0xf 左移 16 位，得到 0xf0000，然后写入到
NVIC_SYSPRI3 寄存器的 bit23-bit16 中。如果要将 SysTick 的优先级设置为最高（0x0），就需要将 0x0 左移 24 位，
得到 0x0，然后写入到 NVIC_SYSPRI3 寄存器的 bit31-bit24 中。这样，NVIC_SYSPRI3 寄存器的值就变成了 0xf0000，
表示 PendSV 和 SysTick 的优先级分别为 0xf 和 0x0。*/

#define portNVIC_SYSPRI2_REG	(*(( volatile uint32_t *) 0xe000ed20))

#define portNVIC_PENDSV_PRI  (((uint32_t) configKERNEL_INTERRUPT_PRIORITY ) <<16UL )
#define portNVIC_SYSTICK_PRI (((uint32_t) configKERNEL_INTERRUPT_PRIORITY) <<24UL ) 
static UBaseType_t uxCriticalNesting = 0xaaaaaaaa;
BaseType_t xPortStartScheduler(void)
{
	uxCriticalNesting = 0;
	portNVIC_SYSPRI2_REG |= portNVIC_PENDSV_PRI;
	portNVIC_SYSPRI2_REG |= portNVIC_SYSTICK_PRI;
	vPortSetupTimerInterrupt();
	prvStartFirstTask();
	return 0;
}

/*svc 0 是一条系统服务调用指令，它会触发一个SVC异常，进入一个SVC异常服务例程，
执行一些系统级别的操作，比如访问受保护的硬件资源或切换特权模式12。

nop 是一条空操作指令，它不会对任何寄存器或内存产生影响，只会消耗一个时钟周期，
通常用来作为占位符或延时3 。

nop nop 是两条连续的空操作指令，它们的作用和一条nop指令相同，只是消耗两个时钟周期，
可能是为了对齐代码或增加延时。*/


void prvStartFirstTask( void )
{
	__asm
	{
		PRESERVE8
		ldr r0, =0xE000ED08
		ldr r0, [r0]
		ldr r0, [r0]
		msr msp, r0
		cpsie i
		cpsie f
		dsb
		isb
		svc 0
		nop
		nop
	}
}


void vPortSVCHandler(void)									
{	
	__asm
	{
		externpxCurrentTCB;
		PRESERVE8
			ldr r3, =pxCurrentTCB
			ldr r1, =[r3]
			ldr r0, =[r1]
			ldmia r0!, { r4 - r11 }
			msr psp.r0
			isb
			mov r0, #0
			msr basepri, r0
			orr r14, #0xd
			bx r14
	}
}

StackType_t* pxPortInitialiseStack(StackType_t* pxTopOfStack,
	TaskFunction_t pxCode,
	void* pvParameters)
{
	pxTopOfStack--;
	*pxTopOfStack = portINITIAL_XPSR;
	pxTopOfStack--;
	*pxTopOfStack = ((StackType_t)pxCode) & portSTART_ADDRESS_MASK;
	pxTopOfStack--;
	*pxTopOfStack = ((StackType_t)prvTaskExitError);
	pxTopOfStack -= 5;
	*pxTopOfStack = ((StackType_t)pvParameters);
	pxTopOfStack -= 8;
	return pxTopOfStack;
}

/*psp是进程堆栈指针（Process Stack Pointer）的缩写，它是ARM Cortex-M内核中的一种特殊的寄存器，
用来存储当前任务的堆栈地址。Cortex-M内核有两个堆栈指针，另一个是主堆栈指针
（Main Stack Pointer），用来存储操作系统内核和异常处理的堆栈地址。
这两个堆栈指针可以通过控制寄存器（CONTROL）来切换，一般情况下，线程模式下使用psp，
异常模式下使用msp12。

psp的作用是为了支持多任务的切换，每个任务都有自己的堆栈空间，当任务切换时，
psp会指向不同的堆栈地址，从而保证任务的上下文不会丢失。psp可以通过MSR/MRS指令来读写，
也可以通过PSPSEL位来选择SP寄存器来访问*/

/*isb，stmdb，ldmia都是arm架构中的汇编指令，它们分别是：

isb（指令同步屏障）：它是一种内存屏障指令，用来确保在isb之前的所有指令都执行完毕，
并且所有影响程序执行的状态都更新到内存中，然后才开始执行isb之后的指令。
它的作用是防止指令乱序执行，保证程序的正确性和一致性12。
stmdb（存储多个数据，递减前）：它是一种多数据传输指令，用来将多个寄存器的值存储到内存中，
从高地址到低地址。它的作用是在每次传输前，将基址寄存器的值减去传输的字节数，
然后将寄存器的值存储到基址寄存器指向的地址中，从右到左的顺序。它的用途是实现堆栈操作，
保护寄存器的值34。
ldmia（加载多个数据，递增后）：它是一种多数据传输指令，用来将内存中的多个数据加载到寄存器中，
从低地址到高地址。它的作用是在每次传输后，将基址寄存器的值加上传输的字节数，
然后将内存中的数据加载到寄存器中，从左到右的顺序。它的用途是实现堆栈操作，恢复寄存器的值34。
这三条指令都是arm架构中的标准指令，它们可以在不同的arm处理器上运行，例如Cortex-M系列。
它们都有一定的格式和约束，例如：

isb的格式是：isb {option}，其中option是可选的，用来指定内存屏障的类型，例如SY（系统屏障），
SH（共享屏障），ISH（内部共享屏障），NSH（非共享屏障）等12。
stmdb的格式是：stmdb{cond} Rn{!}, reglist，其中cond是可选的，用来指定条件码，例如EQ（等于），
NE（不等于），GT（大于），LT（小于）等；Rn是必须的，用来指定基址寄存器，它不能是R15（PC）；
!是可选的，用来指定是否将最后的地址写回基址寄存器；reglist是必须的，
用来指定要存储的寄存器列表，它可以包含一个或多个寄存器，用逗号分隔，例如{R0-R3, R5, LR}34。
ldmia的格式是：ldmia{cond} Rn{!}, reglist，其中cond，Rn，!，reglist的含义和stmdb相同，
只是方向相反*/

/*dsb（数据同步屏障）是一种内存屏障指令，用来确保在dsb之前的所有内存访问指令都执行完毕，
并且所有缓存操作也完成后，才会执行在dsb之后的指令。它的作用是防止指令乱序执行，
保证程序的正确性和一致性12。

bl（分支链接）是一种分支指令，用来实现程序流程的跳转。它的作用是将下一个指令的地址复制到lr
（r14，链接寄存器）中，然后跳转到目标地址继续执行。它的用途是实现函数调用，保留返回地址*/

void xPortPendSVHandler(void)			
{		
	__asm
	{
		extern pxCurrentTCB;
		extern vTaskSwitchContext;
		PRESERVE8
			mrs r0, psp
			isb
			ldr r3, =pxCurrentTCB
			ldr r2, [r3]
			stmdb r0!, { r4 - r11 }
			str r0, [r2]

			stmdb sp!, { r3,r14 }
			mov r0, #configMAX_SYSCALL_INTERRUPT_PRIORITY
			msr basepri, r0
			dsb
			isb
			bl vTaskSwitchContext
			mov r0, #0
			msr basepri, r0
			ldmia sp!, { r3,r14 }
			
			ldr r1, [r3]
			ldr r0, [r1]
			ldmia r0!, { r4 - r11 }
			msr psp, r0
			isb
			bx r14
			nop
	}
}

#define portVECTACTIVE_MASK                   ( 0xFFUL )

void vPortEnterCritical(void)
{
	portDISABLE_INTERRUPTS();
	uxCriticalNesting++;

	if (uxCriticalNesting == 1)
	{
		configASSERT((portNVIC_INT_CTRL_REG & portVECTACTIVE_MASK) == 0);
	}
}

void vPortExitCritical(void)
{
	configASSERT( uxCriticalNesting);
	uxCriticalNesting--;
	if (uxCriticalNesting == 0)
	{
		portENABLE_INTERRUPTS();
	}
}


#define portNVIC_SYSTICK_CTRL_REG		(* ((volatile uint32_t *) 0xe000e010))
#define portNVIC_SYSTICK_LOAD_REG		(* ((volatile uint32_t *) 0xe000e014))

#ifndef configSYSTICK_CLOCK_HZ
#define configSYSTICK_CLOCK_HZ		configCPU_CLOCK_HZ
#define portNVIC_SYSTICK_CLK_BIT		( 1UL << 2UL )
#else
#endif
#define portNVIC_SYSTICK_INT_BIT		( 1UL <<1UL )
#define portNVIC_SYSTICK_ENABLE_BIT		( 1UL <<0UL )

void vPortSetupTimerInterrupt(void)
{
	portNVIC_SYSTICK_LOAD_REG = (portNVIC_SYSTICK_CLK_BIT |
		portNVIC_SYSTICK_INT_BIT |
		portNVIC_SYSTICK_ENABLE_BIT);
}



void xPortSysTickHandler(void)
{
	vPortRaiseBASEPRI();
	if (xTaskIncrementTick() != pdFALSE)
		taskYIELD();

	vPortClearBASEPRIFromISR();


}