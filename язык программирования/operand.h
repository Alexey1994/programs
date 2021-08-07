#ifndef OPERAND_H_INCLUDED
#define OPERAND_H_INCLUDED


#include <data-structures/buffer/buffer.h>


typedef enum
{
	NUMBER,
	LABEL,
	NAME_OPERAND,
	VARIABLE,
	STATIC_STRING,
	//FUNCTION_BODY,
	DATA_BLOCK,
	CALL,
	PRINT,

	NUMBER_LINK,
	//NAME_LINK,
	LABEL_LINK,
	VARIABLE_LINK,
	LABEL_ADDRESS,
	DATA_ADDRESS,
	CAST_TO_NUMBER8,
	CAST_TO_NUMBER16,
	CAST_TO_NUMBER32,
	NUMBER8_FROM_STACK,
	NUMBER16_FROM_STACK,
	NUMBER32_FROM_STACK,
	DATA_BLOCK_ADDRESS,
	POINTER_FROM_STACK,
	UNDEFINED_FROM_STACK
}
Operand_Type;


typedef struct
{
	Operand_Type type;
	Byte         data[];
}
Operand;

typedef struct
{
	N_32 value;
}
Number_Operand;

typedef struct
{
	Buffer name;
}
Name_Operand;

typedef struct
{
	Buffer name;
	N_32   expression_index;
}
Label_Operand;

typedef struct
{
	Buffer name;
	N_32   variable_index;
	Type*  type;
}
Variable_Operand;

typedef struct
{
	Buffer data;
}
Static_String_Operand;

typedef struct
{
	Type*  type;
	Buffer expressions;
}
Call_Operand;

typedef struct
{
	Buffer* expressions;
}
Data_Block_Operand;


Operand* create_number_operand        (N_32 value);
Operand* create_name_operand          (Buffer* name);
Operand* create_label_operand         (Buffer* label_name, N_32 expression_index);
Operand* create_label_link_operand    (Label_Operand* label);
Operand* create_variable_operand      (Buffer* variable_name, N_32 variable_index, Type* type);
Operand* create_variable_link_operand (Variable_Operand* variable);
Operand* create_static_string_operand (Buffer* data);
Operand* create_call_operand          (Type* type);

void         add_operand_in_stack        (Buffer* stack, Operand_Type type, Byte* operand_data);
Operand_Type get_operand_type_from_stack (Buffer* stack);
Operand*     remove_operand_from_stack   (Buffer* stack);


#include "operand.c"
#endif //OPERAND_H_INCLUDED