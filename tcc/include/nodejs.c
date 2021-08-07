#ifndef NODEJS_INCLUDED
#define NODEJS_INCLUDED


#include <types.c>


typedef enum
{
	NODEJS_ok,
	NODEJS_invalid_arg,
	NODEJS_object_expected,
	NODEJS_string_expected,
	NODEJS_name_expected,
	NODEJS_function_expected,
	NODEJS_number_expected,
	NODEJS_boolean_expected,
	NODEJS_array_expected,
	NODEJS_generic_failure,
	NODEJS_pending_exception,
	NODEJS_cancelled,
	NODEJS_escape_called_twice,
	NODEJS_handle_scope_mismatch,
	NODEJS_callback_scope_mismatch,
	NODEJS_queue_full,
	NODEJS_closing,
	NODEJS_bigint_expected,
	NODEJS_date_expected,
	NODEJS_arraybuffer_expected,
	NODEJS_detachable_arraybuffer_expected
}
NodeJS_Status;


typedef struct
{
	Number32 major;
	Number32 minor;
	Number32 patch;
	Byte* release;
}
NodeJS_Version;


import NodeJS_Status napi_get_node_version(void* env, NodeJS_Version** version);


typedef enum
{
	UNDEFINED_NODEJS_TYPE,
	NULL_NODEJS_TYPE,
	BOOLEAN_NODEJS_TYPE,
	NUMBER_NODEJS_TYPE,
	STRING_NODEJS_TYPE,
	SYMBOL_NODEJS_TYPE,
	OBJECT_NODEJS_TYPE,
	FUNCTION_NODEJS_TYPE,
	EXTERNAL_NODEJS_TYPE,
}
NodeJS_Value_Type;


typedef struct
{

}
NodeJS_Value;


typedef struct
{
	Number version;
	Number flags;
	void*  dso_handle;
	Byte*  filename;
	
	//node::addon_register_func nm_register_func;
	//node::addon_context_register_func nm_context_register_func;
	void(*register_func)    (Byte* env, Byte* exports, Byte* data);
	void(*register_context) (Byte* exports, Byte* module, Byte* context, Byte* data);
	
	Byte*                 module_name;
	void*                 data;
	struct NodeJS_Module* link;
}
NodeJS_Module;


import void node_module_register(NodeJS_Module* module);


#endif//NODEJS_INCLUDED