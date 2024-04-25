#ifndef _TASK_H
#define _TASK_H


#include "list.h"
#include "portmarco.h"

/*typedef��#define����������Ϊ�������ͻ������������������������¼��㲻ͬ��

typedef��һ���ؼ��֣���#define��һ��Ԥ����ָ�
typedef�ڱ���׶α���������#define��Ԥ����׶α�������
typedef������������ƣ���#defineû������������ơ�
typedef���Զ�����ƽ̨�޹ص��������ͣ���#define���ܱ�֤��һ�㡣
typedef���Ա�֤��������ı�������ͬһ���ͣ���#define���ܻ�������Ͳ�һ�µ������
typedef���ܶ����е��������������¶��壬��#define���Ը������еĺ�����*/
typedef void* TaskHandle_t;

typedef unsigned int uint32_t;

#define TASK1_STACK_SIZE	128
#define TASK2_STACK_SIZE	128
StackType_t Task1Stack[TASK1_STACK_SIZE];
StackType_t Task2Stack[TASK2_STACK_SIZE];


typedef void* TaskHandle_t;
typedef void (*TaskFunction_t) (void*);

List_t pxReadyTasksLists[configMAX_TASK_NAME_LEN];

#define taskYIELD()		portYIELD( )

#define portNVIC_INT_CTRL_REG		(*(( volatile uint32_t * ) 0xe000ed04))
#define portNVIC_PENDSVSET_BIT		( 1UL << 28UL )
#define portSY_FULL_READ_WRITE		( 15 )

#define portYIELD() \
do\
{ \
portNVIC_INT_CTRL_REG = portNVIC_PENDSVSET_BIT;		\
__dsb(portSY_FULL_READ_WRITE);		\
__isb(portSY_FULL_READ_WRITE);		\
}while(0)

#define taskENTER_CRITICAL()		portENTER_CRITICAL()
#define taskENTER_CRITICAL_FROM_ISR(x)		portSET_INTERRUPT_MASK_FROM_ISR()

#define taskEXIT_CRITCAL()		portEXIT_CRITICAL()
#define taskEXIT_CRITICAL_FROM_ISR(x)		portCLEAR_INTERRUPT_MASK_FROM_ISR(x)

#define taskENTER_CRITICAL_FROM_ISR()		portSET_INTERRUPT_MASK_FROM_ISR()

/*��ARM�ܹ��У�U��һ����׺����ʾ�޷��ţ�unsigned����������ָ��һ����������������������ͣ�
�������������ʽת������������磬1U��ʾһ��32λ���޷������ͳ�������ֵΪ1��1

U��׺�������Ǹ��߱������������������������޷��ŵģ������и������������˿���ʹ��
�����������Χ��ͬʱ����Ҳ���Ժ�������׺��ϣ���ʾ��ͬ���������ͺʹ�С�����磬
1UL��ʾһ��32λ���޷��ų����ͳ�����1ULL��ʾһ��64λ���޷��ų������ͳ���*/

#define	tskIDLE_PRIORITY		( (UBaseType_t) 0U)





#endif









