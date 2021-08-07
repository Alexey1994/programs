void execute(
	Buffer*   expressions,
	Byte*     execution_context,
	void    (*add_operand)(Byte* execution_context, Operand* operand, Operand_Type* result_type, Byte* result, N_32* expression_index),
	void    (*execute_unary_operation)(Byte* execution_context, Byte* operation, Operand* operand, Operand_Type* result_type, Byte* result, N_32* expression_index),
	void    (*execute_binary_operation)(Byte* execution_context, Byte* operation, Operand* left_operand, Operand* right_operand, Operand_Type* result_type, Byte* result, N_32* expression_index),
	void    (*execute_operand_sequence)(Byte* execution_context, Buffer* operands_stack, N_32* expression_index),
	void    (*enter_expression)(Byte* execution_context, Buffer* expression)
)
{
	Operand_Type result_type;
	Byte         result[256];

	N_32 expression_index = 0; //next_expression_index
	N_32 expressions_length = expressions->length / sizeof(Buffer);
	while(expression_index < expressions_length)
	{
		Buffer expression;
		get_buffer_bytes(expressions, expression_index * sizeof(Buffer), &expression, sizeof(Buffer));
		
		if(enter_expression)
		{
			enter_expression(execution_context, &expression);
			set_buffer_bytes(expressions, expression_index * sizeof(Buffer), &expression, sizeof(Buffer));
		}

		++expression_index;
		
	//for_each_buffer_element(expressions, Expression, expression)
		BUFFER(operands_stack, 4)
			for_each_buffer_element(&expression, Expression_Node, node)
			{
				switch(node.type)
				{
					case OPERAND:
					{
						Operand* operand = node.data;
						add_operand(execution_context, operand, &result_type, result, &expression_index);
						add_operand_in_stack(&operands_stack, result_type, result);
						break;
					}

					case UNARY_OPERATION:
					{
						Operand* operand = remove_operand_from_stack(&operands_stack);
						execute_unary_operation(execution_context, node.data, operand, &result_type, result, &expression_index);
						add_operand_in_stack(&operands_stack, result_type, result);
						break;
					}

					case BINARY_OPERATION:
					{
						Operand* right_operand = remove_operand_from_stack(&operands_stack);
						Operand* left_operand = remove_operand_from_stack(&operands_stack);

						execute_binary_operation(execution_context, node.data, left_operand, right_operand, &result_type, result, &expression_index);
						add_operand_in_stack(&operands_stack, result_type, result);
						break;
					}

					default:
						printf("broken data in expression %d %d\n", node.type, node.data);
				}
			}
			end_for_each_buffer_element

			if(operands_stack.length)
				execute_operand_sequence(execution_context, &operands_stack, &expression_index);
		END_BUFFER(operands_stack)
	//end_for_each_buffer_element
	}
}