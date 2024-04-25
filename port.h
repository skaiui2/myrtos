#ifndef _PORT_H
#define _PORT_H

/*pxPortInitialiseStack()*/
#define portINITIAL_XPSR	( 0x01000000)
#define portSTART_ADDRESS_MASK  ( (StackType_t ) 0xfffffffeUL)

StackType_t* pxPortInitialiseStack(StackType_t* pxTopOfStack,
	TaskFunction_t pxCode,
	void* pvParameters);



#endif





