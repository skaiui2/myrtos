/*C语言补充区*/

/* Remove the privileged function macro, but keep the PRIVILEGED_DATA
 * macro so applications can place data in privileged access sections
 * (useful when using statically allocated objects). */
#define PRIVILEGED_FUNCTION
#define PRIVILEGED_DATA    __attribute__( ( section( "privileged_data" ) ) )
#define FREERTOS_SYSTEM_CALL

 /* MPU_WRAPPERS_INCLUDED_FROM_API_FILE */

/* Ensure API functions go in the privileged execution section. */
#define PRIVILEGED_FUNCTION     __attribute__( ( section( "privileged_functions" ) ) )
#define PRIVILEGED_DATA         __attribute__( ( section( "privileged_data" ) ) )
#define FREERTOS_SYSTEM_CALL    __attribute__( ( section( "freertos_system_calls" ) ) )

	/*PRIVILEGED_DATA在FreeRTOS中用于支持内存保护和特权模式的。它的参数是一个属性，用于将
	数据放在特权访问的内存段中2。这样可以防止非特权模式的代码访问或修改内核的数据3。

重点：*************
	__attribute__是一个GCC编译器的扩展，用于为变量，函数，类型等指定一些特殊的属性4。
	section是一个属性，用于指定变量或函数所在的内存段的名字5。privileged_data是一个内存段的
	名字，它是在链接脚本中定义的，用于标记特权访问的数据*/