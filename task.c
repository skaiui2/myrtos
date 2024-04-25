#include "task.h"
#include "rtos.h"
#include "projdefs.h"
#include "portmarco.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "list.h"



/*const是一个关键字，用来修饰变量或对象，表示它们是只读的，不能被修改。const的用法有以下几种：

const变量：定义一个常量，其值在初始化后不能改变。例如，const int a = 10;
const类对象：定义一个类的实例，其成员变量和非const成员函数不能被调用。例如，const string s = "hello";
指向const变量的指针：定义一个指针，指向一个const变量，不能通过指针修改该变量的值。例如，const int *p = &a;
const指针：定义一个指针，其指向不能改变，但可以修改指向的变量的值。例如，int * const q = &b;
*/


/*volatile是一个C/C++语言中的关键字，用来修饰变量或对象，表示它们可能会被某些编译器未知
的因素更改，比如操作系统、硬件或其他线程等。使用volatile可以提醒编译器不对访问这些变量的
代码进行优化，从而保证每次都从内存中读取它们的最新值。

volatile的用法有以下几种场景：

并行设备的硬件寄存器，例如状态寄存器。
中断服务程序中修改的供其他程序检测的变量，例如中断标志位。
多任务环境下各任务间共享的标志，例如信号量。
存储器映射的硬件寄存器，例如I/O端口。
使用volatile时，需要注意以下几个问题：

volatile只能保证可见性，不能保证原子性，因此在多线程环境下，可能还需要加锁或同步机制。
volatile不能修饰常量，因为常量的值在初始化后就不能改变。

   ********  volatile不能保证指针的指向不变，只能保证指针指向的变量的值不变。***********
   
volatile可能会增加代码的尺寸和降低性能，因此要合理地使用volatile。*/
typedef unsigned int uint32_t;

#if(configSUPPORT_STATIC_ALLOCATION == 1)
TaskHandle_t xTaskCreateStatic(TaskFunction_t pxTaskCode,
	const char* const pcName,
	const uint32_t ulStackDepth,
	void* const pvParameters,
	UBaseType_t uxPriority,
	StackType_t* const puxStackBuffer,
	TCB_t* const pxTaskBuffer)
{
	TCB_t* pxNewTCB;
	TaskHandle_t xReturn;
	if ((pxTaskBuffer != NULL) && (puxStackBuffer != NULL))
	{
		pxNewTCB = (TCB_t*)pxTaskBuffer;
		pxNewTCB->pxStack = (StackType_t*)puxStackBuffer;
		prvInitialiseNewTask(pxTaskCode,
			pcName,
			ulStackDepth,
			pvParameters,
			uxPriority,
			&xReturn,
			pxNewTCB);
		prvAddNewTaskToReadyList(pxNewTCB);
	}
	else
	{
		xReturn = NULL;
	}
	return xReturn;
}
#endif

static void prvInitialiseNewTask(TaskFunction_t pxTaskCode,
	const char* const pcName,
	const uint32_t ulStackDepth,
	void* const pvParameters,
	UBaseType_t uxPriority,
	TaskHandle_t* const pxCreatTask,
	TCB_t* pxNewTCB)
{
	StackType_t* pxTopOfStack;
	UBaseType_t x;
	pxTopOfStack = pxNewTCB->pxStack + (ulStackDepth - (uint32_t)1);
	pxTopOfStack = (StackType_t*)\
		(((uint32_t)pxTopOfStack) & (~((uint32_t)0x0007)));
	for (x = (UBaseType_t)0; x < (UBaseType_t)configMAX_TASK_NAME_LEN; x++)
	{
		pxNewTCB->pcTaskName[x] == pcName[x];
		if (pcName[x] == 0x00)
			break;
	}
	pxNewTCB->pcTaskName[configMAX_TASK_NAME_LEN - 1] = '\0';
	vListInitialiseItem(&(pxNewTCB->xStateListItem));
	listSET_LIST_ITEM_OWNER(&(pxNewTCB->xStateListItem), pxNewTCB);
	if (uxPriority >= (UBaseType_t)configMAX_PRIORITIES)
	{
		uxPriority = (UBaseType_t)configMAX_PRIORITIES - (UBaseType_t)1U;
	}
	pxNewTCB->uxPriority = uxPriority;

	pxNewTCB->pxTopOfStack = pxPortInitialiseStack(pxTopOfStack,
		pxTaskCode,
		pvParameters);
	if ((void*)pxCreatTask != NULL)
	{
		*pxCreatTask = (TaskHandle_t)pxNewTCB;
	}
}

static TaskHandle_t xIdleTaskHandle = NULL;

TaskFunction_t prvIdleTask;
static volatile TickType_t xNextTaskUnblockTime = (TickType_t)0U;
void vTaskStartScheduler(void)
{

	//pxCurrentTCB是指向当前或下一个任务的指针，改变指向实现任务切换
	TCB_t* pxIdleTaskTCBBuffer = NULL;
	StackType_t* pxIdleTaskStackBuffer = NULL;
	uint32_t ulIdleTaskStackSize;
	vApplicationGetIdleTaskMemory(&pxIdleTaskTCBBuffer,
		&pxIdleTaskStackBuffer,
		&ulIdleTaskStackSize);
	xIdleTaskHandle = xTaskCreateStatic( (TaskFunction_t)prvIdleTask,
		(char*) "IDLE",
		(uint32_t) ulIdleTaskStackSize,
		(void*) NULL,
		(UBaseType_t)tskIDLE_PRIORITY,
		(StackType_t*) pxIdleTaskStackBuffer,
		(TCB_t*)pxIdleTaskTCBBuffer
		);

	xNextTaskUnblockTime = portMAX_DELAY;
	xTickCount = (TickType_t)0U;
	if (xPortStartScheduler() != pdFALSE)
	{

	}
}



extern IdleTaskTCB;
TCB_t Task1TCB;
TCB_t Task2TCB;

/*#if 0是一个预处理器指令，用于在编译时控制源文件中的一部分代码是否被编译。如果你在
一段代码前面加上#if 0，那么这段代码就不会被编译，相当于被注释掉了。如果你想让这段代码被编译，
你可以把#if 0改成#if 1，或者删除它。这个指令通常用于调试或测试的目的，可以方便地开启或关闭
一些功能*/
#if 1
void vTaskSwitchContext(void)
{
	
	taskSELECT_HIGHEST_PRIORITY_TASK();
}
#else

void vTaskSwitchContext(void)
{
	if (Task1TCB.xTicksToDelay == 0)
		pxCurrentTCB = &Task1TCB;
	else if (Task2TCB.xTicksToDelay == 0)
		pxCurrentTCB = &Task2TCB;
	else if (pxCurrentTCB == &Task1TCB && Task2TCB.xTicksToDelay != 0)
		pxCurrentTCB = &IdleTaskTCB;
	else if (pxCurrentTCB == &Task2TCB && Task1TCB.xTicksToDelay != 0)
		pxCurrentTCB = &IdleTaskTCB;
	else
		return;
}

#endif


/*左移是一种位运算，用来将一个二进制数的各位向左移动若干位，右边空出的位用0填补，
左边溢出的位舍弃。左移相当于将原数乘以2的移动位数次方。例如，32左移2位，就是将
32的二进制表示0010 0000向左移动2位，得到1000 0000，再转换成十进制，就是128。也就是说，
32左移2位相当于32乘以2的2次方，即32乘以4，等于12812。
左移的符号是<<，左边的操作数是要移动的二进制数，右边的操作数是要移动的位数。
例如，a << b，就是将a的二进制数向左移动b位。左移的优点是可以快速地进行乘法运
算，提高效率*/

#define taskRESET_READY_PRIORITY( uxPriority )	 portRESET_READY_PRIORITY( uxPriority, uxTopReadyPriority )

#define taskRECORD_READY_PRIORITY( uxPriority )    portRECORD_READY_PRIORITY( ( uxPriority ), uxTopReadyPriority )

/*-----------------------------------------------------------*/
void vTaskDelay(const TickType_t xTicksToDelay)
{
	TCB_t* pxTCB = NULL;
	pxTCB = pxCurrentTCB;
	pxTCB->xTicksToDelay = xTicksToDelay;
	prvAddCurrentTaskToDelayedList(xTicksToDelay);
	taskYIELD();
}

static volatile TickType_t xTickCount = (TickType_t)0;

#define configMAX_PRIORITIES  16
List_t pxReadyTasksLists[configMAX_PRIORITIES];



static volatile BaseType_t xNumOfOverflows = (BaseType_t)0;
#define taskSWITCH_DELAYED_LISTS()\
{ \
	List_t* pxTemp;\
	pxTemp = pxDelayedTaskList;\
	pxDelayedTaskList = pxOverflowDelayedTaskList;\
	pxOverflowDelayedTaskList = pxTemp;\
	xNumOfOverflows++;\
	prvResetNextTaskUnblockTime();\
}


BaseType_t xTaskIncrementTick(void)
{
	TCB_t* pxTCB ;
	BaseType_t xItemValue ;
	BaseType_t xSwitchRequired = pdFALSE;
	const TickType_t xConstTickCount = xTickCount + 1;
	xTickCount = xConstTickCount;

	if (xConstTickCount == (TickType_t)0U)
	{
		taskSWITCH_DELAYED_LISTS();
	}
	if (xConstTickCount >= xNextTaskUnblockTime)
	{
		for (; ;)
		{
			if (listLIST_IS_EMPTY(pxDelayedTaskList) != pdFALSE)
			{
				xNextTaskUnblockTime = portMAX_DELAY;
				break;
			}
			else
			{
				pxTCB = (TCB_t*)listGET_OWNER_OF_HEAD_ENTRY(pxDelayedTaskList);
				xItemValue = listGET_LIST_ITEM_VALUE(&(pxTCB->xStateListItem));
				if (xConstTickCount < xItemValue)
				{
					xNextTaskUnblockTime = xItemValue;
					break;
				}
			}
			(void)uxListRemove(&(pxTCB->xStateListItem));
			/*下面这个是宏函数，定义有点复杂，涉及其他宏函数，回头我再自行查阅*/
			prvAddTaskToReadyList(pxTCB);

#if( configUSE_PREEMPTION == 1)
			{
				if (pxTCB->uxPriority >= pxCurrentTCB->uxPriority)
				{
					xSwitchRequired = pdTRUE;
				}
			}
#endif
		}
	}
#if( (configUSE_PREEMPTION == 1) && (configUSE_TIME_SLICING == 1))
	{
		if (listCURRENT_LIST_LENGTH(&(pxReadyTasksLists[pxCurrentTCB->uxPriority])) > (UBaseType_t)1)
		{
			xSwitchRequired = pdTRUE;
		}
	}
#endif
	/*portYIELD函数在FreeRTOS中用于强制进行任务切换的。这个宏函数的实现是通过向NVIC_INT_CTRL_REG寄存器
	写入PENDSVSET_BIT来触发PendSV异常，然后在异常处理函数中调用vTaskSwitchContext()来切换到下一个
	就绪任务2。__dsb()和__isb()是两个内嵌汇编函数，用于保证数据同步和指令同步*/
	portYIELD();
}



/*a==  如果a没有定义，编译器貌似默认为a=0*/
#if( configUSE_PORT_OPTIMISED_TASK_SELECTION == 0)
#define taskRECORD_READY_PRIORITY(uxPriority)\
{\
	if( ( uxPriority ) > uxTopReadyPriority )\
	{\
		uxTopReadyPriority = (uxPriority);\
	}\
}

#define taskSELECT_HIGHEST_PRIORITY_TASK()\
{\
	while( listLIST_IS_EMPTY( &(pxReadyTasksLists[ uxTopPriority ])))\
	{\
		--uxTopPriority;\
	}\
	listGET_OWNER_OF_NEXT_ENTRY(pxCurrentTCB, &(pxReadyTasksLists[uxTopPriority]));\
	uxTopReadyPriority = uxTopPriority;\
}
#define taskRESET_READY_PRIORITY( uxPriority )
#define portRESET_PRIORITY(uxPriority ,uxTopReadyPriority )

#else
#define taskRECORD_READY_PRIORITY( uxPriority )\
		portRECORD_READY_PRIORITY(uxPriority, uxTopReadyPriority )

#define taskSELECT_HIGHEST_PRIORITY_TASK()\
{\
	UBaseType_t uxTopPriority;\
	portGET_HIGHEST_PRIORITY(uxTopPriority, uxTopReadyPriority);\
	listGET_OWNER_OF_NEXT_ENTRY(pxCurrentTCB, &( pxReadyTasksLists[ uxTopPriority ]);\
}
#endif

#if 1
/*宏定义的功能是：

首先，它调用 listCURRENT_LIST_LENGTH() 函数，获取指定优先级的就绪任务列表中的任务数量。2
然后，它判断该数量是否为零，即该优先级下是否没有就绪的任务。3
如果是，它调用 portRESET_READY_PRIORITY() 函数，清除优先级位图表 uxTopReadyPriority 中
相应的位，表示该优先级下没有就绪的任务。
这个宏定义的作用是为了优化任务调度的效率，避免在没有就绪任务的优先级下浪费时间*/
#define taskRESET_READY_PRIORITY( uxPriority )\
{\
	if (listCURRENT_LIST_LENGTH(&(pxReadyTasksLists[uxPriority])) == (UBaseType_t)0)\
	{\
		portRESET_READY_PRIORITY((uxPriority), (uxTopReadyPriority));\
	}\
}
#else
#define taskRESET_READY_PRIORITY(uxPriority)\
{\
	portRESET_READY_PRIORITY((uxPriority), (uxTopReadyPriority));\
}
#endif



static volatile UBaseType_t uxTopReadyPriority = tskIDLE_PRIORITY;


#define prvAddTaskToReadyList( pxTCB )\
		taskRECORD_READY_PRIORITY( ( pxTCB )-> uxPriority );\
		vListInsertEnd( & (pxReadyTasksLists[ (pxTCB)-> uxPriority]),&( ( pxTCB )->xStateListItem));

static volatile UBaseType_t uxCurrentNumberOfTasks = (UBaseType_t)0U;
static void prvAddNewTaskToReadyList(TCB_t* pxNewTCB)
{
	taskENTER_CRITICAL();
	{
		uxCurrentNumberOfTasks++;
		if (pxCurrentTCB == NULL)
		{
			pxCurrentTCB = pxNewTCB;
			if (uxCurrentNumberOfTasks == (UBaseType_t)1)
				prvInitialiseTaskLists();
		}
		else
		{
			if (pxCurrentTCB->uxPriority <= pxNewTCB->uxPriority)
				pxCurrentTCB = pxNewTCB;
		}
		prvAddTaskToReadyList(pxNewTCB);
	}
	taskEXIT_CRITCAL();
}

void prvInitialiseTaskLists(void)
{
	UBaseType_t uxPriority;
	for (uxPriority = (UBaseType_t)0U; uxPriority < (UBaseType_t)configMAX_PRIORITIES; uxPriority++)
	{
		vListInitialise(&(pxReadyTasksLists[uxPriority]));
	}
	vListInitialise(&xDelayedTaskList1);
	vListInitialise(&xDelayedTaskList2);
	pxDelayedTaskList = &xDelayedTaskList1;
	pxOverflowDelayedTaskList = &xDelayedTaskList2;
}

static List_t xDelayedTaskList1;
static List_t xDelayedTaskList2;
static List_t* volatile pxDelayedTaskList;
static List_t* volatile pxOverflowDelayedTaskList;

static void prvAddCurrentTaskToDelayedList(TickType_t xTicksToWait)
{
	TickType_t xTimeToWake;
	const TickType_t xConstTickCount = xTickCount;
	if (uxListRemove(&(pxCurrentTCB->xStateListItem)) == (UBaseType_t)0)
	{
		portRESET_READY_PRIORITY(pxCurrentTCB->uxPriority, uxTopReadyPriority);
	}
	xTimeToWake = xConstTickCount + xTicksToWait;
	listSET_LIST_ITEM_VALUE(&(pxCurrentTCB->xStateListItem), xTimeToWake);
	if (xTimeToWake < xConstTickCount)
	{
		vListInsert(pxOverflowDelayedTaskList, &(pxCurrentTCB->xStateListItem));
	}
	else
	{
		vListInsert(pxDelayedTaskList, &(pxCurrentTCB->xStateListItem));
		if (xTimeToWake < xNextTaskUnblockTime)
		{
			xNextTaskUnblockTime = xTimeToWake;
		}
	}
}



static void prvResetNextTaskUnblockTime()
{
	TCB_t* pxTCB;
	if (listLIST_IS_EMPTY(pxDelayedTaskList) != pdFALSE)
	{
		xNextTaskUnblockTime = portMAX_DELAY;
	}
	else
	{
		(pxTCB) = (TCB_t*)listGET_OWNER_OF_HEAD_ENTRY(pxDelayedTaskList);
		xNextTaskUnblockTime = listGET_LIST_ITEM_VALUE(&((pxTCB)->xStateListItem));
	}
}

