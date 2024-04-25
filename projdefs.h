#ifndef PROJDEFS_H
#define PROJDEFS_H

#include "stdint.h"
#include "stddef.h"
#include "task.h"
#include "port.h"




typedef void (*TaskFunction_t)(void*);
#define pdFALSE		((BaseType_t )0)
#define pdTRUE		((BaseType_t )1)


#define pdPASS		(pdTRUE)
#define pdFAIL		(pdFALSE)


#endif

