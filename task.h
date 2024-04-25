#ifndef _TASK_H
#define _TASK_H


#include "list.h"
#include "portmarco.h"

/*typedef和#define都可以用来为数据类型或对象定义别名，但是它们有以下几点不同：

typedef是一个关键字，而#define是一个预处理指令。
typedef在编译阶段被解析，而#define在预处理阶段被解析。
typedef有作用域的限制，而#define没有作用域的限制。
typedef可以定义与平台无关的数据类型，而#define不能保证这一点。
typedef可以保证连续定义的变量都是同一类型，而#define可能会出现类型不一致的情况。
typedef不能对已有的类型名进行重新定义，而#define可以覆盖已有的宏名。*/
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

/*在ARM架构中，U是一个后缀，表示无符号（unsigned）。它用于指定一个常量或操作数的数据类型，
避免编译器的隐式转换或溢出。例如，1U表示一个32位的无符号整型常量，其值为1。1

U后缀的作用是告诉编译器，这个常量或操作数是无符号的，不会有负数的情况，因此可以使用
更大的正数范围。同时，它也可以和其他后缀组合，表示不同的数据类型和大小。例如，
1UL表示一个32位的无符号长整型常量，1ULL表示一个64位的无符号长长整型常量*/

#define	tskIDLE_PRIORITY		( (UBaseType_t) 0U)





#endif









