#ifndef EXPRESSION_PARSER_INCLUDED
#define EXPRESSION_PARSER_INCLUDED


#include <data-structures/stack.c>
#include <reader.c>


typedef enum
{
	OPERAND                 = 1,
	OPEN_BRACE              = 2,
	CLOSE_BRACE             = 3,
	UNARY_OPERATION         = 4,
	POSTFIX_UNARY_OPERATION = 5,
	BINARY_OPERATION        = 6
}
Expression_Node_Type;

typedef struct
{
	Expression_Node_Type type;
	Byte*                data;
}
Expression_Node;


typedef struct
{
	Byte*  data;
	Number priority;
}
Operation;


static void add_operation_in_stack(Stack* stack, Expression_Node_Type type, Operation* operation)
{
	Expression_Node node;
	node.type = type;
	node.data = operation;
	add_bytes_in_stack(stack, &node, sizeof(Expression_Node));
}

static Expression_Node remove_operation_from_stack(Stack* stack)
{
	Expression_Node node;
	remove_bytes_from_stack(stack, &node, sizeof(Expression_Node));
	return node;
}

static Expression_Node get_operation_from_stack(Stack* stack)
{
	return *(Expression_Node*)(stack->data + stack->size - sizeof(Expression_Node));
}

static void add_expression_node(Stack* expression, Expression_Node_Type type, Byte* data)
{
	Expression_Node node;
	node.type = type;
	node.data = data;
	add_bytes_in_stack(expression, &node, sizeof(Expression_Node));
}


Boolean parse_expression(
	Stack*  expression,
	Reader* reader,
	Byte*   parser,

	Operation* (*read_operand)(Byte* parser),
	Operation* (*read_unary_operation)(Byte* parser),
	Operation* (*read_postfix_unary_operation)(Byte* parser),
	Operation* (*read_binary_operation)(Byte* parser)
)
{
	Stack           stack;
	Expression_Node operation;
	Operation*      unary_operation;
	Operation*      postfix_unary_operation;
	Operation*      binary_operation;
	Byte*           operand;
	Boolean         require_operand;

	initialize_stack(&stack, 128);
	initialize_stack(expression, 128);

	read_spaces(reader);
	require_operand = 1;

	while(!end_of_reader(reader))
	{
		switch(get_reader_byte(reader))
		{
			case '(':
				if(!require_operand)
					goto end_expression;

				read_next_byte(reader);
				add_operation_in_stack(&stack, OPEN_BRACE, 0);
				add_expression_node(expression, OPEN_BRACE, 0);
				require_operand = 1;
				break;

			case ')':
				if(require_operand)
					goto error;

				read_next_byte(reader);

				while(stack.size && get_operation_from_stack(&stack).type != OPEN_BRACE)
				{
					operation = remove_operation_from_stack(&stack);
					add_expression_node(expression, operation.type, operation.data);
				}

				if(!stack.size)
					goto error;

				remove_operation_from_stack(&stack);
				add_expression_node(expression, CLOSE_BRACE, 0);
				require_operand = 0;
				break;

			default:
				if(require_operand)
				{
					unary_operation = read_unary_operation(parser);

					if(unary_operation)
					{
						add_operation_in_stack(&stack, UNARY_OPERATION, unary_operation);
						require_operand = 1;
						break;
					}
				}

				if(require_operand)
				{
					operand = read_operand(parser);

					if(!operand)
						goto error;

					add_expression_node(expression, OPERAND, operand);
					require_operand = 0;
				}
				else
				{
					postfix_unary_operation = read_postfix_unary_operation(parser);

					if(postfix_unary_operation)
					{
						while(stack.size)
						{
							operation = get_operation_from_stack(&stack);
							if(operation.type != OPEN_BRACE && ((Operation*)operation.data)->priority <= postfix_unary_operation->priority)
							{
								operation = remove_operation_from_stack(&stack);
								add_expression_node(expression, operation.type, operation.data);
							}
							else
								break;
						}

						add_expression_node(expression, POSTFIX_UNARY_OPERATION, postfix_unary_operation);
					}
					else
					{
						binary_operation = read_binary_operation(parser);

						if(!binary_operation)
							goto end_expression;

						while(stack.size)
						{
							operation = get_operation_from_stack(&stack);
							if(operation.type != OPEN_BRACE && ((Operation*)operation.data)->priority <= binary_operation->priority)
							{
								operation = remove_operation_from_stack(&stack);
								add_expression_node(expression, operation.type, operation.data);
							}
							else
								break;
						}

						add_operation_in_stack(&stack, BINARY_OPERATION, binary_operation);
						require_operand = 1;
					}
				}
		}

		read_spaces(reader);
	}

end_expression:
	while(stack.size)
	{
		operation = remove_operation_from_stack(&stack);

		if(operation.type == OPEN_BRACE)
			goto error;

		add_expression_node(expression, operation.type, operation.data);
	}

	deinitialize_stack(&stack);
	return 1;

error:
	deinitialize_stack(&stack);
	deinitialize_stack(expression);
	return 0;
}


Byte* execute_expression(
	Stack* expression,
	Byte*  context,

	void (*execute_operand)(Byte* context, Byte* operand),
	Byte* (*execute_unary_operation)(Byte* context, Operation* operation, Byte* operand),
	Byte* (*execute_postfix_unary_operation)(Byte* context, Operation* operation, Byte* operand),
	Byte* (*execute_binary_operation)(Byte* context, Operation* operation, Byte* operand1, Byte* operand2)
)
{
	Number           i;
	Expression_Node* node;
	Stack            stack;
	Byte*            operand1;
	Byte*            operand2;
	Byte*            result;

	initialize_stack(&stack, 128);

	node = expression->data;
	for(i = 0; i < expression->size / sizeof(Expression_Node); ++i)
	{
		switch(node->type)
		{
			case OPERAND:
				execute_operand(context, node->data);
				add_bytes_in_stack(&stack, &node->data, sizeof(node->data));
				break;

			case UNARY_OPERATION:
				remove_bytes_from_stack(&stack, &operand1, sizeof(operand1));
				result = execute_unary_operation(context, node->data, operand1);
				add_bytes_in_stack(&stack, &result, sizeof(result));
				break;

			case POSTFIX_UNARY_OPERATION:
				remove_bytes_from_stack(&stack, &operand1, sizeof(operand1));
				result = execute_postfix_unary_operation(context, node->data, operand1);
				add_bytes_in_stack(&stack, &result, sizeof(result));
				break;

			case BINARY_OPERATION:
				remove_bytes_from_stack(&stack, &operand2, sizeof(operand2));
				remove_bytes_from_stack(&stack, &operand1, sizeof(operand1));
				result = execute_binary_operation(context, node->data, operand1, operand2);
				add_bytes_in_stack(&stack, &result, sizeof(result));
				break;
		}

		++node;
	}

	result = *(Byte**)stack.data;
	deinitialize_stack(&stack);

	return result;
}


void print_expression_in_postfix_notation(
	Stack* expression,
	Byte*  context,

	void (*print_operand)(Byte* context, Byte* operand),
	void (*print_unary_operation)(Byte* context, Operation* operation),
	void (*print_postfix_unary_operation)(Byte* context, Operation* operation),
	void (*print_binary_operation)(Byte* context, Operation* operation)
)
{
	Number           i;
	Expression_Node* node;

	node = expression->data;
	for(i = 0; i < expression->size / sizeof(Expression_Node); ++i)
	{
		switch(node->type)
		{
			case OPERAND:
				print_operand(context, node->data);
				break;

			case OPEN_BRACE:
				print("( ");
				break;

			case CLOSE_BRACE:
				print(") ");
				break;

			case UNARY_OPERATION:
				print_unary_operation(context, node->data);
				break;

			case POSTFIX_UNARY_OPERATION:
				print_postfix_unary_operation(context, node->data);
				break;

			case BINARY_OPERATION:
				print_binary_operation(context, node->data);
				break;
		}

		++node;
	}
}


#endif // EXPRESSION_PARSER_INCLUDED

/*
#include <reader/bytes.c>


Number read_operand(Reader* reader)
{
	Number number = get_reader_byte(reader);

	if(number >= '0' && number <= '9')
	{
		read_next_byte(reader);
		read_spaces(reader);
		return number;
	}

error:
	return 0;
}

Operation unary_operations[] = {
	{'-', 1}
};

Operation* read_unary_operation(Reader* reader)
{
	if(read_if(reader, "-"))
		return unary_operations;

	return 0;
}

Operation postfix_unary_operations[] = {
	{']', 10}
};

Operation* read_postfix_unary_operation(Reader* reader)
{
	if(read_if(reader, "]"))
		return postfix_unary_operations;

	return 0;
}

Operation binary_operations[] = {
	{'+', 2},
	{'-', 2},
	{'*', 1},
	{'/', 1}
};

Operation* read_binary_operation(Reader* reader)
{
	if(read_if(reader, "+"))
		return binary_operations;

	if(read_if(reader, "-"))
		return binary_operations + 1;

	if(read_if(reader, "*"))
		return binary_operations + 2;

	if(read_if(reader, "/"))
		return binary_operations + 3;

	return 0;
}


typedef struct
{
	Stack stack;
}
Execution;

void print_operand(Execution* execution, Byte operand)
{
	printf("%c ", operand);
	
	operand -= '0';
	add_bytes_in_stack(&execution->stack, &operand, 1);
}

void print_unary_operation(Execution* execution, Operation* operation)
{
	printf("unary%c ", operation->data);

	Integer_Number8 operand;

	remove_bytes_from_stack(&execution->stack, &operand, 1);

	switch(operation->data)
	{
		case '-':
			operand = -operand;
			add_bytes_in_stack(&execution->stack, &operand, 1);
			break;
	}
}

void print_postfix_unary_operation(Execution* execution, Operation* operation)
{
	printf("%c ", operation->data);

	Integer_Number8 operand;

	remove_bytes_from_stack(&execution->stack, &operand, 1);

	switch(operation->data)
	{
		case '-':
			operand = -operand;
			add_bytes_in_stack(&execution->stack, &operand, 1);
			break;
	}
}

void print_binary_operation(Execution* execution, Operation* operation)
{
	printf("%c ", operation->data);

	Integer_Number8 left_operand;
	Integer_Number8 right_operand;

	remove_bytes_from_stack(&execution->stack, &right_operand, 1);
	remove_bytes_from_stack(&execution->stack, &left_operand, 1);

	switch(operation->data)
	{
		case '+':
			left_operand += right_operand;
			add_bytes_in_stack(&execution->stack, &left_operand, 1);
			break;

		case '-':
			left_operand -= right_operand;
			add_bytes_in_stack(&execution->stack, &left_operand, 1);
			break;

		case '*':
			left_operand *= right_operand;
			add_bytes_in_stack(&execution->stack, &left_operand, 1);
			break;

		case '/':
			if(right_operand)
				left_operand /= right_operand;
			else
			{
				left_operand = -1;
				left_operand = left_operand >> 1;
			}
			add_bytes_in_stack(&execution->stack, &left_operand, 1);
			break;
	}
}


void main()
{
	Reader          reader;
	Stack           expression;
	Execution       execution;
	Integer_Number8 result;

	initialize_null_terminated_bytes_reader(&reader,
		"2 + 3 + 4\n"
		"2 * 3 + 4\n"
	);

	initialize_stack(&execution.stack, 128);

	if(parse_expression(
		&expression,
		&reader,
		&reader,

		&read_operand,
		&read_unary_operation,
		&read_postfix_unary_operation,
		&read_binary_operation
	))
	{
		print_expression_in_postfix_notation(
			&expression,
			&execution,
			&print_operand,
			&print_unary_operation,
			&print_postfix_unary_operation,
			&print_binary_operation
		);

		remove_bytes_from_stack(&execution.stack, &result, 1);
		printf("= %d\n", result);
	}

	if(parse_expression(
		&expression,
		&reader,
		&reader,

		&read_operand,
		&read_unary_operation,
		&read_postfix_unary_operation,
		&read_binary_operation
	))
	{
		print_expression_in_postfix_notation(
			&expression,
			&execution,
			&print_operand,
			&print_unary_operation,
			&print_postfix_unary_operation,
			&print_binary_operation
		);

		remove_bytes_from_stack(&execution.stack, &result, 1);
		printf("= %d\n", result);
	}
}*/