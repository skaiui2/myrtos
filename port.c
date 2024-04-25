#include "port.h"
#include "task.h"
#include "rtos.h"
#include "projdefs.h"
#include "portmarco.h"
#include "list.h"

/*configKERNEL_INTERRUPT_PRIORITY��һ���궨�壬��������FreeRTOS�ں��Լ����ж����ȼ���
��ΪFreeRTOS�ں��жϲ�������ռ�û�ʹ�õ��жϣ���������һ�㶨��ΪӲ��������ȼ�12��

��ARM Cortex-M�ں��У������ϵĵ����ȼ�ֵ������ָ���߼��ϵĸ��ж����ȼ������磬
һ���������������ȼ�Ϊ2���жϵ��߼����ȼ�����һ���������������ȼ�Ϊ5���жϵ��߼����ȼ���
��ˣ�configKERNEL_INTERRUPT_PRIORITY��ֵӦ���������ϵ����ֵ������2553��

��FreeRTOSConfig.h�ļ��У����Կ���configKERNEL_INTERRUPT_PRIORITY�Ķ������£�

#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY 0xf
#define configKERNEL_INTERRUPT_PRIORITY ( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )

���У�configLIBRARY_LOWEST_INTERRUPT_PRIORITY��Ӳ��������ȼ���ֵ��
configPRIO_BITS�ǿ��õ����ȼ�λ����������ȡ���ھ����΢������4��ͨ�����Ʋ�����
��configLIBRARY_LOWEST_INTERRUPT_PRIORITY��ֵת��ΪARM Cortex-M�ں˱�����Ҫ�����ȼ�ֵ��
���Ѿ��ƶ����ֽڵ������Чλ3��*/

static void* prvTaskExitError(void)
{
	for (;;);
}




/*portNVIC_PENDSV_PRI����������PendSV���ж����ȼ��ģ�����configKERNEL_INTERRUPT_PRIORITY
����16λ��Ȼ�󸳸�NVIC_SYSPRI3�Ĵ�����bit23-bit16�������Ϳ��Կ���PendSV�����ȼ���
PendSV��һ���ɹ����ϵͳ���ã�����FreeRTOS���������������л���

portNVIC_SYSTICK_PRI����������SysTick���ж����ȼ��ģ�
����configKERNEL_INTERRUPT_PRIORITY����24λ��
Ȼ�󸳸�NVIC_SYSPRI3�Ĵ�����bit31-bit24�������Ϳ��Կ���SysTick�����ȼ���
SysTick��һ��ϵͳ�δ�ʱ��������FreeRTOS���������������л�����ʱ��*/

/*PendSV��SysTick��Cortex-M�ں��е������쳣��������FreeRTOS������ʵ��������Ⱥ��л���

PendSV���ɹ����ϵͳ���ã����쳣���Ϊ14���ɱ�̡������������������л��ģ�
���������״̬�����仯ʱ���ͻᴥ��PendSV�쳣��ͨ���ı����ջָ�루PSP���л�����ͬ������
PendSV�쳣�����ȼ�����͵ģ������������������жϴ�����ɺ��ִ�У�
�������Ա�֤����ϵͳ��ʵʱ�ԡ�

SysTick��ϵͳ�δ�ʱ�������쳣���Ϊ15���ɱ�̡�����������Ϊ����ϵͳ������ʱ�ӵģ�
һ��Ĭ��Ϊ1ms��ÿ���ж�ʱ��ϵͳ�����Ƿ����µľ���������Ҫ���У�����У�
�ͻ�����PendSV�쳣���Ա㻺��ִ���������л�15��SysTick�����ȼ�һ������Ϊ��ͣ�
�������û����ⲿ�ж�26��
PendSV �� SysTick �� ARM Cortex-M �ں��е������쳣�������� FreeRTOS ������ʵ��������Ⱥ��л���
���ǵ��ж����ȼ������ǲ��� 0xE000_ED20 ��ַ�ģ������ַ�� NVIC_SYSPRI3 �Ĵ����ĵ�ַ��
�������洢 PendSV��SysTick �� Debug Monitor �����ȼ�34��

NVIC_SYSPRI3 �Ĵ�����һ�� 32 λ�����ݣ����� bit31-bit24 �洢 SysTick �����ȼ���
bit23-bit16 �洢 PendSV �����ȼ���bit15-bit8 �洢 Debug Monitor �����ȼ���bit7-bit0 ����34��
��ˣ�Ҫ���� PendSV �� SysTick �����ȼ�������Ҫ�����ȼ�ֵ���Ƶ���Ӧ��λ�ã�Ȼ��д�뵽 NVIC_SYSPRI3 �Ĵ����С�

���磬���Ҫ�� PendSV �����ȼ�����Ϊ��ͣ�0xf��������Ҫ�� 0xf ���� 16 λ���õ� 0xf0000��Ȼ��д�뵽
NVIC_SYSPRI3 �Ĵ����� bit23-bit16 �С����Ҫ�� SysTick �����ȼ�����Ϊ��ߣ�0x0��������Ҫ�� 0x0 ���� 24 λ��
�õ� 0x0��Ȼ��д�뵽 NVIC_SYSPRI3 �Ĵ����� bit31-bit24 �С�������NVIC_SYSPRI3 �Ĵ�����ֵ�ͱ���� 0xf0000��
��ʾ PendSV �� SysTick �����ȼ��ֱ�Ϊ 0xf �� 0x0��*/

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

/*svc 0 ��һ��ϵͳ�������ָ����ᴥ��һ��SVC�쳣������һ��SVC�쳣�������̣�
ִ��һЩϵͳ����Ĳ�������������ܱ�����Ӳ����Դ���л���Ȩģʽ12��

nop ��һ���ղ���ָ���������κμĴ������ڴ����Ӱ�죬ֻ������һ��ʱ�����ڣ�
ͨ��������Ϊռλ������ʱ3 ��

nop nop �����������Ŀղ���ָ����ǵ����ú�һ��nopָ����ͬ��ֻ����������ʱ�����ڣ�
������Ϊ�˶�������������ʱ��*/


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

/*psp�ǽ��̶�ջָ�루Process Stack Pointer������д������ARM Cortex-M�ں��е�һ������ļĴ�����
�����洢��ǰ����Ķ�ջ��ַ��Cortex-M�ں���������ջָ�룬��һ��������ջָ��
��Main Stack Pointer���������洢����ϵͳ�ں˺��쳣����Ķ�ջ��ַ��
��������ջָ�����ͨ�����ƼĴ�����CONTROL�����л���һ������£��߳�ģʽ��ʹ��psp��
�쳣ģʽ��ʹ��msp12��

psp��������Ϊ��֧�ֶ�������л���ÿ���������Լ��Ķ�ջ�ռ䣬�������л�ʱ��
psp��ָ��ͬ�Ķ�ջ��ַ���Ӷ���֤����������Ĳ��ᶪʧ��psp����ͨ��MSR/MRSָ������д��
Ҳ����ͨ��PSPSELλ��ѡ��SP�Ĵ���������*/

/*isb��stmdb��ldmia����arm�ܹ��еĻ��ָ����Ƿֱ��ǣ�

isb��ָ��ͬ�����ϣ�������һ���ڴ�����ָ�����ȷ����isb֮ǰ������ָ�ִ����ϣ�
��������Ӱ�����ִ�е�״̬�����µ��ڴ��У�Ȼ��ſ�ʼִ��isb֮���ָ�
���������Ƿ�ָֹ������ִ�У���֤�������ȷ�Ժ�һ����12��
stmdb���洢������ݣ��ݼ�ǰ��������һ�ֶ����ݴ���ָ�����������Ĵ�����ֵ�洢���ڴ��У�
�Ӹߵ�ַ���͵�ַ��������������ÿ�δ���ǰ������ַ�Ĵ�����ֵ��ȥ������ֽ�����
Ȼ�󽫼Ĵ�����ֵ�洢����ַ�Ĵ���ָ��ĵ�ַ�У����ҵ����˳��������;��ʵ�ֶ�ջ������
�����Ĵ�����ֵ34��
ldmia�����ض�����ݣ������󣩣�����һ�ֶ����ݴ���ָ��������ڴ��еĶ�����ݼ��ص��Ĵ����У�
�ӵ͵�ַ���ߵ�ַ��������������ÿ�δ���󣬽���ַ�Ĵ�����ֵ���ϴ�����ֽ�����
Ȼ���ڴ��е����ݼ��ص��Ĵ����У������ҵ�˳��������;��ʵ�ֶ�ջ�������ָ��Ĵ�����ֵ34��
������ָ���arm�ܹ��еı�׼ָ����ǿ����ڲ�ͬ��arm�����������У�����Cortex-Mϵ�С�
���Ƕ���һ���ĸ�ʽ��Լ�������磺

isb�ĸ�ʽ�ǣ�isb {option}������option�ǿ�ѡ�ģ�����ָ���ڴ����ϵ����ͣ�����SY��ϵͳ���ϣ���
SH���������ϣ���ISH���ڲ��������ϣ���NSH���ǹ������ϣ���12��
stmdb�ĸ�ʽ�ǣ�stmdb{cond} Rn{!}, reglist������cond�ǿ�ѡ�ģ�����ָ�������룬����EQ�����ڣ���
NE�������ڣ���GT�����ڣ���LT��С�ڣ��ȣ�Rn�Ǳ���ģ�����ָ����ַ�Ĵ�������������R15��PC����
!�ǿ�ѡ�ģ�����ָ���Ƿ����ĵ�ַд�ػ�ַ�Ĵ�����reglist�Ǳ���ģ�
����ָ��Ҫ�洢�ļĴ����б������԰���һ�������Ĵ������ö��ŷָ�������{R0-R3, R5, LR}34��
ldmia�ĸ�ʽ�ǣ�ldmia{cond} Rn{!}, reglist������cond��Rn��!��reglist�ĺ����stmdb��ͬ��
ֻ�Ƿ����෴*/

/*dsb������ͬ�����ϣ���һ���ڴ�����ָ�����ȷ����dsb֮ǰ�������ڴ����ָ�ִ����ϣ�
�������л������Ҳ��ɺ󣬲Ż�ִ����dsb֮���ָ����������Ƿ�ָֹ������ִ�У�
��֤�������ȷ�Ժ�һ����12��

bl����֧���ӣ���һ�ַ�ָ֧�����ʵ�ֳ������̵���ת�����������ǽ���һ��ָ��ĵ�ַ���Ƶ�lr
��r14�����ӼĴ������У�Ȼ����ת��Ŀ���ַ����ִ�С�������;��ʵ�ֺ������ã��������ص�ַ*/

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