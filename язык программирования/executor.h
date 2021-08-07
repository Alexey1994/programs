#ifndef EXECUTOR_H_INCLUDED
#define EXECUTOR_H_INCLUDED


//#include "parser.h"


void execute(
	Buffer*   expressions,
	Byte*     execution_context,
	void    (*add_operand)(Byte* execution_context, Operand* operand, Operand_Type* result_type, Byte* result, N_32* expression_index),
	void    (*execute_unary_operation)(Byte* execution_context, Byte* operation, Operand* operand, Operand_Type* result_type, Byte* result, N_32* expression_index),
	void    (*execute_binary_operation)(Byte* execution_context, Byte* operation, Operand* left_operand, Operand* right_operand, Operand_Type* result_type, Byte* result, N_32* expression_index),
	void    (*execute_operand_sequence)(Byte* execution_context, Buffer* operands_stack, N_32* expression_index),
	void    (*enter_expression)(Byte* execution_context, Buffer* expression)
);


#include "executor.c"
#endif //EXECUTOR_H_INCLUDED