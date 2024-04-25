#include "task.h"
#include "rtos.h"
#include "projdefs.h"
#include "portmarco.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "list.h"



/*const��һ���ؼ��֣��������α�������󣬱�ʾ������ֻ���ģ����ܱ��޸ġ�const���÷������¼��֣�

const����������һ����������ֵ�ڳ�ʼ�����ܸı䡣���磬const int a = 10;
const����󣺶���һ�����ʵ�������Ա�����ͷ�const��Ա�������ܱ����á����磬const string s = "hello";
ָ��const������ָ�룺����һ��ָ�룬ָ��һ��const����������ͨ��ָ���޸ĸñ�����ֵ�����磬const int *p = &a;
constָ�룺����һ��ָ�룬��ָ���ܸı䣬�������޸�ָ��ı�����ֵ�����磬int * const q = &b;
*/


/*volatile��һ��C/C++�����еĹؼ��֣��������α�������󣬱�ʾ���ǿ��ܻᱻĳЩ������δ֪
�����ظ��ģ��������ϵͳ��Ӳ���������̵߳ȡ�ʹ��volatile�������ѱ��������Է�����Щ������
��������Ż����Ӷ���֤ÿ�ζ����ڴ��ж�ȡ���ǵ�����ֵ��

volatile���÷������¼��ֳ�����

�����豸��Ӳ���Ĵ���������״̬�Ĵ�����
�жϷ���������޸ĵĹ�����������ı����������жϱ�־λ��
�����񻷾��¸�����乲��ı�־�������ź�����
�洢��ӳ���Ӳ���Ĵ���������I/O�˿ڡ�
ʹ��volatileʱ����Ҫע�����¼������⣺

volatileֻ�ܱ�֤�ɼ��ԣ����ܱ�֤ԭ���ԣ�����ڶ��̻߳����£����ܻ���Ҫ������ͬ�����ơ�
volatile�������γ�������Ϊ������ֵ�ڳ�ʼ����Ͳ��ܸı䡣

   ********  volatile���ܱ�ָ֤���ָ�򲻱䣬ֻ�ܱ�ָ֤��ָ��ı�����ֵ���䡣***********
   
volatile���ܻ����Ӵ���ĳߴ�ͽ������ܣ����Ҫ�����ʹ��volatile��*/
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

	//pxCurrentTCB��ָ��ǰ����һ�������ָ�룬�ı�ָ��ʵ�������л�
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

/*#if 0��һ��Ԥ������ָ������ڱ���ʱ����Դ�ļ��е�һ���ִ����Ƿ񱻱��롣�������
һ�δ���ǰ�����#if 0����ô��δ���Ͳ��ᱻ���룬�൱�ڱ�ע�͵��ˡ������������δ��뱻���룬
����԰�#if 0�ĳ�#if 1������ɾ���������ָ��ͨ�����ڵ��Ի���Ե�Ŀ�ģ����Է���ؿ�����ر�
һЩ����*/
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


/*������һ��λ���㣬������һ�����������ĸ�λ�����ƶ�����λ���ұ߿ճ���λ��0���
��������λ�����������൱�ڽ�ԭ������2���ƶ�λ���η������磬32����2λ�����ǽ�
32�Ķ����Ʊ�ʾ0010 0000�����ƶ�2λ���õ�1000 0000����ת����ʮ���ƣ�����128��Ҳ����˵��
32����2λ�൱��32����2��2�η�����32����4������12812��
���Ƶķ�����<<����ߵĲ�������Ҫ�ƶ��Ķ����������ұߵĲ�������Ҫ�ƶ���λ����
���磬a << b�����ǽ�a�Ķ������������ƶ�bλ�����Ƶ��ŵ��ǿ��Կ��ٵؽ��г˷���
�㣬���Ч��*/

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
			/*��������Ǻ꺯���������е㸴�ӣ��漰�����꺯������ͷ�������в���*/
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
	/*portYIELD������FreeRTOS������ǿ�ƽ��������л��ġ�����꺯����ʵ����ͨ����NVIC_INT_CTRL_REG�Ĵ���
	д��PENDSVSET_BIT������PendSV�쳣��Ȼ�����쳣�������е���vTaskSwitchContext()���л�����һ��
	��������2��__dsb()��__isb()��������Ƕ��ຯ�������ڱ�֤����ͬ����ָ��ͬ��*/
	portYIELD();
}



/*a==  ���aû�ж��壬������ò��Ĭ��Ϊa=0*/
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
/*�궨��Ĺ����ǣ�

���ȣ������� listCURRENT_LIST_LENGTH() ��������ȡָ�����ȼ��ľ��������б��е�����������2
Ȼ�����жϸ������Ƿ�Ϊ�㣬�������ȼ����Ƿ�û�о���������3
����ǣ������� portRESET_READY_PRIORITY() ������������ȼ�λͼ�� uxTopReadyPriority ��
��Ӧ��λ����ʾ�����ȼ���û�о���������
����궨���������Ϊ���Ż�������ȵ�Ч�ʣ�������û�о�����������ȼ����˷�ʱ��*/
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

