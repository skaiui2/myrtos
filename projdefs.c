/*C���Բ�����*/

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

	/*PRIVILEGED_DATA��FreeRTOS������֧���ڴ汣������Ȩģʽ�ġ����Ĳ�����һ�����ԣ����ڽ�
	���ݷ�����Ȩ���ʵ��ڴ����2���������Է�ֹ����Ȩģʽ�Ĵ�����ʻ��޸��ں˵�����3��

�ص㣺*************
	__attribute__��һ��GCC����������չ������Ϊ���������������͵�ָ��һЩ���������4��
	section��һ�����ԣ�����ָ�������������ڵ��ڴ�ε�����5��privileged_data��һ���ڴ�ε�
	���֣����������ӽű��ж���ģ����ڱ����Ȩ���ʵ�����*/