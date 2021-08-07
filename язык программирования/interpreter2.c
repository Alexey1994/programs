#include <log/log.h>
#include "executor.h"


static void interprete_unary_operation(
	Generator* generator,
	Byte* operation,
	Operand* operand,
	Operand_Type* result_type, Byte* result
)
{
	*result_type = UNDEFINED_FROM_STACK;

	switch(operation)
	{
		case 'N':
		{
			*result_type = CAST_TO_NUMBER32;
			break;
		}

		case '-':
		{
			N_32 value;
			remove_bytes_from_buffer_end(&generator->execution_stack, &value, sizeof(value));

			switch(operand->type)
			{
				case NUMBER:
				case NUMBER32_FROM_STACK:
				case LABEL_LINK:
					value = -value;
					add_in_buffer_end(&generator->execution_stack, N_32, value);
					break;

				case VARIABLE_LINK:
					get_buffer_bytes(&generator->execution_stack, value * sizeof(N_32), &value, sizeof(value));
					value = -value;
					add_in_buffer_end(&generator->execution_stack, N_32, value);
					break;

				default:
					printf("error: undefined operand type (%d) in unary - operation\n", operand->type);
			}

			*result_type = NUMBER32_FROM_STACK;
			break;
		}

		case 'p':
		{
			N_32 value;
			remove_bytes_from_buffer_end(&generator->execution_stack, &value, sizeof(value));

			switch(operand->type)
			{
				case NUMBER:
				case NUMBER32_FROM_STACK:
					printf("%d\n", value);
					break;

				case VARIABLE_LINK:
					get_buffer_bytes(&generator->execution_stack, value * sizeof(N_32), &value, sizeof(value));
					printf("%d\n", value);
					break;

				case LABEL_LINK:
					printf("&%d\n", value);
					break;

				case DATA_ADDRESS:
					printf("%d\n", value);
					//printf("%d %d\n", value, *((N_32*)value));
					break;

				case DATA_BLOCK_ADDRESS:
					printf("[ ");
					cycle(0, value, 1)
						printf("%2.2x ", generator->execution_stack.data[generator->execution_stack.length - value + i]);
					end
					printf("]");

					printf(" %d bytes\n", value);
					generator->execution_stack.length -= value;
					break;

				default:
					printf("error: undefined operand type (%d) in print operation\n", operand->type);
			}
			break;
		}

		default:
			if(((N_32)operation) & 0b10000000000000000000000000000000)
				*result_type = CAST_TO_NUMBER32;
			else
				printf("error: undefined unary operation %c\n", operation);
	}
}


static void interprete_binary_operation(
	Generator* generator,
	Byte* operation,
	Operand* left_operand,
	Operand* right_operand,
	Operand_Type* result_type, Byte* result,
	N_32* expression_index
)
{
	*result_type = UNDEFINED_FROM_STACK;

	N_32 value;
	N_32 right_value;
	remove_bytes_from_buffer_end(&generator->execution_stack, &right_value, sizeof(right_value));

	switch(right_operand->type)
	{
		case NUMBER:
		case NUMBER32_FROM_STACK:
		case LABEL_LINK:
		case DATA_BLOCK_ADDRESS:
			break;

		case VARIABLE_LINK:
			get_buffer_bytes(&generator->execution_stack, right_value * sizeof(N_32), &right_value, sizeof(right_value));
			break;

		default:
			printf("error: undefined right operand type (%d) in binary operation\n", right_operand->type);
	}

	switch(operation)
	{
		case '=':
		{
			N_32 left_value;
			remove_bytes_from_buffer_end(&generator->execution_stack, &left_value, sizeof(left_value));

			switch(left_operand->type)
			{
				case VARIABLE_LINK:
					set_buffer_bytes(&generator->execution_stack, left_value * sizeof(N_32), &right_value, sizeof(right_value));
					break;

				default:
					printf("error: undefined left operand type (%d) in = operation\n", left_operand->type);
			}

			break;
		}

		case '+':
		{
			N_32 left_value;
			remove_bytes_from_buffer_end(&generator->execution_stack, &left_value, sizeof(left_value));

			switch(left_operand->type)
			{
				case NUMBER:
				case NUMBER32_FROM_STACK:
				case LABEL_LINK:
					value = left_value + right_value;
					add_in_buffer_end(&generator->execution_stack, N_32, value);
					*result_type = NUMBER32_FROM_STACK;
					break;

				case VARIABLE_LINK:
					get_buffer_bytes(&generator->execution_stack, left_value * sizeof(N_32), &left_value, sizeof(left_value));
					value = left_value + right_value;
					add_in_buffer_end(&generator->execution_stack, N_32, value);
					*result_type = NUMBER32_FROM_STACK;
					break;

				default:
					printf("error: undefined left operand type (%d) in + operation\n", left_operand->type);
			}

			break;
		}

		case '-':
		{
			N_32 left_value;
			remove_bytes_from_buffer_end(&generator->execution_stack, &left_value, sizeof(left_value));

			switch(left_operand->type)
			{
				case NUMBER:
				case NUMBER32_FROM_STACK:
				case LABEL_LINK:
					value = left_value - right_value;
					add_in_buffer_end(&generator->execution_stack, N_32, value);
					*result_type = NUMBER32_FROM_STACK;
					break;

				case VARIABLE_LINK:
					get_buffer_bytes(&generator->execution_stack, left_value * sizeof(N_32), &left_value, sizeof(left_value));
					value = left_value - right_value;
					add_in_buffer_end(&generator->execution_stack, N_32, value);
					*result_type = NUMBER32_FROM_STACK;
					break;

				default:
					printf("error: undefined left operand type (%d) in - operation\n", left_operand->type);
			}

			break;
		}

		case '*':
		{
			N_32 left_value;
			remove_bytes_from_buffer_end(&generator->execution_stack, &left_value, sizeof(left_value));

			switch(left_operand->type)
			{
				case NUMBER:
				case NUMBER32_FROM_STACK:
				case LABEL_LINK:
					value = left_value * right_value;
					add_in_buffer_end(&generator->execution_stack, N_32, value);
					*result_type = NUMBER32_FROM_STACK;
					break;

				case VARIABLE_LINK:
					get_buffer_bytes(&generator->execution_stack, left_value * sizeof(N_32), &left_value, sizeof(left_value));
					value = left_value * right_value;
					add_in_buffer_end(&generator->execution_stack, N_32, value);
					*result_type = NUMBER32_FROM_STACK;
					break;

				default:
					printf("error: undefined left operand type (%d) in * operation\n", left_operand->type);
			}

			break;
		}

		case '/':
		{
			N_32 left_value;
			remove_bytes_from_buffer_end(&generator->execution_stack, &left_value, sizeof(left_value));

			switch(left_operand->type)
			{
				case NUMBER:
				case NUMBER32_FROM_STACK:
				case LABEL_LINK:
					value = left_value / right_value;
					add_in_buffer_end(&generator->execution_stack, N_32, value);
					*result_type = NUMBER32_FROM_STACK;
					break;

				case VARIABLE_LINK:
					get_buffer_bytes(&generator->execution_stack, left_value * sizeof(N_32), &left_value, sizeof(left_value));
					value = left_value / right_value;
					add_in_buffer_end(&generator->execution_stack, N_32, value);
					*result_type = NUMBER32_FROM_STACK;
					break;

				default:
					printf("error: undefined left operand type (%d) in / operation\n", left_operand->type);
			}

			break;
		}

		case 'i':
		{
			N_32 left_value;
			remove_bytes_from_buffer_end(&generator->execution_stack, &left_value, sizeof(left_value));

			if(right_value)
			{
				switch(left_operand->type)
				{
					case NUMBER:
					case NUMBER32_FROM_STACK:
					case LABEL_LINK:
						*expression_index = left_value;
						break;

					case VARIABLE_LINK:
						get_buffer_bytes(&generator->execution_stack, left_value * sizeof(N_32), &left_value, sizeof(left_value));
						*expression_index = left_value;
						break;

					default:
						printf("error: undefined left operand type (%d) in if operation\n", left_operand->type);
				}
			}
			break;
		}

		default:
			printf("error: undefined binary operation %c\n", operation);
	}
}


static void interprete_operand_sequence(Generator* generator, Buffer* operands_stack, N_32* expression_index)
{

}


static void add_interpreter_operand(Generator* generator, Operand* operand, Operand_Type* result_type, Byte* result)
{
	*result_type = UNDEFINED_FROM_STACK;

	switch(operand->type)
	{
		case NUMBER:
		{
			Number_Operand* number_operand = &operand->data;

			add_in_buffer_end(&generator->execution_stack, N_32, number_operand->value);

			Number_Operand* number_result = result;
			number_result->value = number_operand->value;
			*result_type = NUMBER;
			break;
		}

		case LABEL:
		case VARIABLE:
			break;

		case LABEL_LINK:
		{
			Label_Operand** label_operand = &operand->data;

			add_in_buffer_end(&generator->execution_stack, N_32, (*label_operand)->expression_index);

			Label_Operand** label_result = result;
			*label_result = *label_operand;
			*result_type = LABEL_LINK;
			break;
		}

		case VARIABLE_LINK:
		{
			Variable_Operand** variable_operand = &operand->data;

			add_in_buffer_end(&generator->execution_stack, N_32, (*variable_operand)->variable_index);

			Variable_Operand** variable_result = result;
			*variable_result = *variable_operand;
			*result_type = VARIABLE_LINK;
			break;
		}

		case CALL:
		{
			Call_Operand* call_operand = &operand->data;
			interprete(call_operand->type);
		}

/*
		case NAME_OPERAND:
		{
			Name_Operand* name_operand = &operand->data;

			Name_Operand* name_result = result;
			*name_result = *name_operand;
			*result_type = NAME_OPERAND;
			break;
		}

		case DATA_BLOCK:
		{
			Data_Block_Operand* data_block_operand = &operand->data;

			N_32 start_address_offset = generator->execution_stack.length;

			execute(
				data_block_operand->expressions,
				generator,
				&add_interpreter_operand,
				&interprete_unary_operation,
				&interprete_binary_operation,
				&interprete_operand_sequence,
				0
			);

			N_32 data_length = generator->execution_stack.length - start_address_offset;
			add_in_buffer_end(&generator->execution_stack, N_32, data_length);

			*result_type = DATA_BLOCK_ADDRESS;

			break;
		}*/

		default:
		{
			printf("error: undefined operand type: %d\n", operand->type);
		}
	}
}


void interprete(Type* type)
{
	Generator generator;

	generator.current_type = type;

	initialize_buffer(&generator.execution_stack, 1024);

	cycle(0, type->variables.length / sizeof(Variable_Operand*), 1)
		add_in_buffer_end(&generator.execution_stack, N_32, 0);
	end

	generator.base_address = 0;

	execute(
		&type->expressions,
		&generator,
		&add_interpreter_operand,
		&interprete_unary_operation,
		&interprete_binary_operation,
		&interprete_operand_sequence,
		0
	);
}