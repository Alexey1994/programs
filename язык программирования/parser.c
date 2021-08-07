#include <memory/memory.h>

void print_token(Buffer* token);

//#include "variables-calculator.h"
//#include "links-calculator.h"
#include "file/file.h"


Z_32 compare_UTF8_string(Buffer* string1, Buffer* string2);



Label_Operand* find_label(Parser* parser, Buffer* name)
{
	for_each_buffer_element(&parser->current_type.labels, Operand*, operand)
		Label_Operand* label = &operand->data;

		if(!compare_UTF8_string(&label->name, name))
			return label;
	end_for_each_buffer_element

	return 0;
}


Type_Node* get_type_node(Parser* parser, N_32 type_index, Buffer* node_name)
{
	/*Buffer*    type;
	Type_Node* node;
	N_32       node_index;

	type = parser->types.data + type_index * sizeof(Buffer);

	for(node = type->data, node_index = 0; node_index < type->length / sizeof(Type_Node); ++node, ++node_index)
	{
		if(!compare_UTF8_string(&node->name, node_name))
			return node;
	}*/

	return 0;
}


N_32 get_type_node_index(Parser* parser, N_32 type_index, Buffer* node_name)
{
	/*Buffer*    type;
	Type_Node* node;
	N_32       node_index;

	type = parser->types.data + type_index * sizeof(Buffer);

	for(node = type->data, node_index = 0; node_index < type->length / sizeof(Type_Node); ++node, ++node_index)
	{
		if(!compare_UTF8_string(&node->name, node_name))
			return node_index;
	}*/

	return -1;
}


/*
Function_Body_Operand* create_function_body_operand(Buffer* expressions)
{
	Function_Body_Operand* operand;

	operand = allocate_memory(sizeof(Function_Body_Operand));
	operand->type = operand->type2 = FUNCTION_BODY;
	initialize_buffer(&operand->local_variables, 20);
	operand->expressions = expressions;

	return operand;
}
*/

Call_Operand* create_call_operand(Buffer* name)
{
	Call_Operand* operand;

	operand = allocate_memory(sizeof(Call_Operand));
	operand->type = operand->type2 = CALL;

	initialize_buffer(&operand->name, 10);
	add_buffer_in_buffer_end(&operand->name, name);

	return operand;
}


N_32 read_next_UTF_8_character_from_string(N_8** string)
{
	N_8  current_byte;
	N_32 number_of_bytes;
	N_32 result;

	current_byte = **string;
	++*string;

	if(!(current_byte & 0b10000000))
		return current_byte;

	if((current_byte & 0b11110000) == 0b11110000)
	{
		number_of_bytes = 4;
		result = (current_byte & 0b00001111) << 18;
	}
	else if((current_byte & 0b11100000) == 0b11100000)
	{
		number_of_bytes = 3;
		result = (current_byte & 0b00011111) << 12;
	}
	else if((current_byte & 0b11000000) == 0b11000000)
	{
		number_of_bytes = 2;
		result = (current_byte & 0b00111111) << 6;
	}
	else
		goto error;

	cycle(0, number_of_bytes - 1, 1)
	current_byte = **string;
	++*string;
	result |= (current_byte & 0b00111111) << ((number_of_bytes - 2 - i) * 6);
	end

	return result;

error:
	return 1;
}


Boolean is_latin_character(N_32 character)
{
	return character >= 'a' && character <= 'z' || character >= 'A' && character <= 'Z';
}


Boolean is_cyrillic_character(N_32 character)
{
	return character >= 0x0410 && character <= 0x044F || character == 0x0451 || character == 0x0401;
}

/*
Boolean is_number_character(N_32 character)
{
	return character >= '0' && character <= '9';
}*/


N_32 read_token(Buffer* token, Reader* reader)
{
	N_16 character;

	clear_buffer(token);

	if(end_of_reader(reader))
		goto error;

	character = get_reader_UTF_8_character(reader);

	if(!is_latin_character(character) && !is_cyrillic_character(character) && character != '_')
		goto error;

	read_UTF_8_character(reader);
	add_in_buffer_end(token, N_16, character);

	while(!end_of_reader(reader))
	{
		character = get_reader_UTF_8_character(reader);

		if(!is_latin_character(character) && !is_cyrillic_character(character) && !is_number_character(character) && character != '_')
			break;

		character = read_UTF_8_character(reader);
		add_in_buffer_end(token, N_16, character);
	}

	return 1;

error:
	return 0;
}


Z_32 compare_token(Buffer* token, N_8* string)
{
	N_16* token_data;
	N_32  token_length;
	Z_32  result;

	token_data = token->data;
	token_length = token->length / 4;

	for(; token_length && *string; --token_length, ++token_data)
	{
		result = *token_data - read_next_UTF_8_character_from_string(&string);

		if(result)
			return result;
	}

	if(*string)
		return read_next_UTF_8_character_from_string(&string);
	else if(token_length)
		return *token_data;

	return 0;
}


Z_32 compare_UTF8_string(Buffer* string1, Buffer* string2)
{
	N_16* string1_data;
	N_32  string1_length;
	N_16* string2_data;
	N_32  string2_length;
	Z_32  result;

	string1_data = string1->data;
	string1_length = string1->length / 4;
	string2_data = string2->data;
	string2_length = string2->length / 4;

	for(; string1_length && string2_length; --string1_length, ++string1_data, --string2_length, ++string2_data)
	{
		result = *string1_data - *string2_data;

		if(result)
			return result;
	}

	if(string1_length)
		return 1;

	if(string2_length)
		return -1;

	return 0;
}


void print_token(Buffer* token)
{
	for_each_buffer_element(token, N_16, character)
		printf("%c", character);
	end_for_each_buffer_element
}


Boolean read_expression(Buffer* expression, Parser* parser);


Boolean read_data_block(Parser* parser, Buffer* expressions)
{
	Expression expression;

	next_reader_byte(&parser->reader);
	initialize_buffer(expressions, 100);

	while(!end_of_reader(&parser->reader) && get_reader_byte(&parser->reader) != ']')
	{
		if(!read_expression(&expression.expression_data, parser))
			goto error;

		add_in_buffer_end(expressions, Expression, expression);
	}

	if(get_reader_byte(&parser->reader) != ']')
	{
		printf("expected ]\n");
		goto error;
	}

	next_reader_byte(&parser->reader);

	return 1;

error:
	deinitialize_buffer(&expressions);
	return 0;
}


Byte* parse_expression_operand(Parser* parser)
{
	N_8   operand_type;
	Byte* operand;

	operand = 0;

	read_token(&parser->token, &parser->reader);
	read_spaces(&parser->reader);

	if(parser->token.length)
	{
		Boolean end_of_reader_data = end_of_reader(&parser->reader);

		if(!end_of_reader_data)
		{
			N_32 character = get_reader_UTF_8_character(&parser->reader);

			if(character == ':')
			{
				read_UTF_8_character(&parser->reader);

				if(find_label(parser, &parser->token))
				{
					printf("error: label ");
					print_token(&parser->token);
					printf("was defined\n");
				}
				else
				{
					operand = create_label_operand(&parser->token, parser->current_expression_index);
					add_in_buffer_end(&parser->current_type.labels, Label_Operand*, operand);
				}
			}
			else if(character == '(')
			{
				read_UTF_8_character(&parser->reader);
				read_UTF_8_character(&parser->reader);
				operand = create_call_operand(&parser->token);
			}
			else
			{
				operand = create_name_operand(&parser->token);
			}
		}
		else
		{
			operand = create_name_operand(&parser->token);
		}
	}
	else
	{
		operand_type = get_reader_byte(&parser->reader);

		if(is_number_character(operand_type))
		{
			operand = create_number_operand(read_N_32(&parser->reader));
		}
		else
		{
			switch(operand_type)
			{
			case '[':
			{
				Buffer* expressions = allocate_memory(sizeof(Buffer));
				
				if(read_data_block(parser, expressions))
					operand = create_data_block_operand(expressions);
				break;
			}

			case '"':
				next_reader_byte(&parser->reader);

				N_32 character;
				Buffer string_data;

				initialize_buffer(&string_data, 4);

				BUFFER(string_data, 4)
				{
					while(!end_of_reader(&parser->reader))
					{
						character = read_UTF_8_character(&parser->reader);

						if(character == '\\')
						{
							character = read_UTF_8_character(&parser->reader);
						}

						if(character == '"')
						{
							next_reader_byte(&parser->reader);
							break;
						}

						add_in_buffer_end(&string_data, N_32, character);
					}

					operand = create_static_string_operand(&string_data);
				}
				END_BUFFER(string_data)
				break;

			default:
				printf("undefined character: %c [character code: %d]\n", operand_type, operand_type);
			}
		}
	}

	read_spaces(&parser->reader);

	return operand;

error:
	return 0;
}


Boolean read_if_operation(Reader* reader, Byte* operation_name)
{
	Boolean result;
	Byte    byte;

	result = 1;
	ACCUMULATED_READER(reader)
		if(!read_if(&reader, operation_name))
		{
			result = 0;
			restore_reader;
		}

		byte = get_reader_byte(&reader);

		if(is_latin_character(byte) || is_number_character(byte))
		{
			result = 0;
			restore_reader;
		}
		else
		{
			add_in_buffer_begin(&_target_reader->buffer, Byte, byte);
		}
	END_ACCUMULATED_READER

	return result;
}


Byte* read_unary_operation(Parser* parser)
{
	Byte byte;

	byte = get_reader_byte(&parser->reader);

	switch(byte)
	{
		case '-':
		case '!':
			next_reader_byte(&parser->reader);
			return byte;

		case 'p':
		{
			if(read_if_operation(&parser->reader, "print"))
				return 'p';

			break;
		}

		case 'N':
		{
			if(read_if_operation(&parser->reader, "Number"))
				return 'N';

			break;
		}
	}

	Boolean is_function_link;
	ACCUMULATED_READER(&parser->reader)
		BUFFER(token, 100)
			read_token(&token, &reader);
			add_in_buffer_end(&token, N_16, 0);
			
			is_function_link = parse_type(token.data, parser->types);

			if(!is_function_link)
			{
				restore_reader;
			}
		END_BUFFER(token)
	END_ACCUMULATED_READER

	return is_function_link;
}


Byte* read_binary_operation(Parser* parser)
{
	Byte byte;

	byte = get_reader_byte(&parser->reader);

	switch(byte)
	{
		case '=':
		case '+':
		case '-':
		case '/':
		case '*':
		case '.':
			next_reader_byte(&parser->reader);
			return byte;

		case 'i':
		{
			if(read_if_operation(&parser->reader, "if"))
				return 'i';
			break;
		}
	}

	return 0;
}


Boolean read_expression(Buffer* expression, Parser* parser)
{
	return parse_expression(
		expression,
		&parser->reader,
		parser,
		&parse_expression_operand,
		&read_unary_operation,
		&read_binary_operation
	);
}


#include "executor.h"


void add_variable(Parser* parser, Buffer* variable_name, N_32 variable_type_index)
{
	Variable_Operand* variable;

	variable = create_variable_operand(variable_name, parser->current_type.variables.length / sizeof(Variable_Operand*), variable_type_index);
	add_in_buffer_end(&parser->current_type.variables, Operand*, variable);
}


Variable_Operand* find_variable(Parser* parser, Buffer* name)
{
	for_each_buffer_element(&parser->current_type.variables, Operand*, operand)
		Variable_Operand *variable = &operand->data;

		if(!compare_UTF8_string(&variable->name, name))
			return variable;
	end_for_each_buffer_element

	return 0;
}


static void add_variables_calculator_operand(Parser* parser, Operand* operand, Operand_Type* result_type, Byte* result)
{
	*result_type = UNDEFINED_FROM_STACK;

	switch(operand->type)
	{
		case NAME_OPERAND:
		{
			Name_Operand* name_operand = &operand->data;
			
			Name_Operand* name_result = result;
			*name_result = *name_operand;
			*result_type = NAME_OPERAND;
			break;
		}
	}
}


static void execute_variables_calculator_unary_operation(
	Parser* parser,
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
			if(operand->type != NAME_OPERAND)
			{
				printf("error: variable definition\n");
				parser->has_error = 1;
				break;
			}

			Name_Operand* name_operand = &operand->data;

			if(find_variable(parser, &name_operand->name))
			{
				printf("error: variable ");
				print_token(&name_operand->name);
				printf(" was defined\n");
				parser->has_error = 1;
				break;
			}

			add_variable(parser, &name_operand->name, NUMBER);

			*result_type = CAST_TO_NUMBER32;
			break;
		}

		default:
		{
			/*
			if(((N_32)operation) & 0b10000000000000000000000000000000)
			{
				//N_32 type_index = ((N_32)operation) & 0b01111111111111111111111111111111;

				if(operand->type != NAME_OPERAND)
				{
					printf("error: variable definition\n");
					parser->has_error = 1;
					break;
				}

				Name_Operand* name_operand = &operand->data;

				if(find_variable(parser, &name_operand->name))
				{
					printf("error: variable ");
					print_token(&name_operand->name);
					printf(" was defined\n");
					parser->has_error = 1;
					break;
				}

				add_variable(parser, &name_operand->name, operation);

				*result_type = CAST_TO_NUMBER32;
			}
			else
				*result_type = UNDEFINED_FROM_STACK;*/
		}
	}
}


static void execute_variables_calculator_binary_operation(
	Parser* parser,
	Byte* operation,
	Operand* left_operand,
	Operand* right_operand,
	Operand_Type* result_type, Byte* result
)
{
	*result_type = UNDEFINED_FROM_STACK;
}


static void execute_variables_calculator_operand_sequence(Parser* parser, Buffer* operands_stack)
{

}


Boolean calculate_variables(Parser* parser)
{
	execute(
		&parser->expressions,
		parser,
		&add_variables_calculator_operand,
		&execute_variables_calculator_unary_operation,
		&execute_variables_calculator_binary_operation,
		&execute_variables_calculator_operand_sequence,
		0
	);

	return 1;
}


void calculate_links(Parser* parser)
{
	Expression new_expression;

	for_each_buffer_element(&parser->expressions, Expression, expression)
		initialize_buffer(&new_expression.expression_data, 100);
		
		for_each_buffer_element(&expression.expression_data, Expression_Node, node)
		{
			switch(node.type)
			{
				case OPERAND:
				{
					Operand* operand = node.data;

					switch(operand->type)
					{
						case NAME_OPERAND:
						{
							Name_Operand* name_operand = &operand->data;
							Label_Operand* label;

							label = find_label(parser, &name_operand->name);
								
							if(label)
							{
								add_expression_node(&new_expression.expression_data, OPERAND, create_label_link_operand(label));	
							}
							else
							{
								Variable_Operand* variable = find_variable(parser, &name_operand->name);

								if(variable)
								{
									add_expression_node(&new_expression.expression_data, OPERAND, create_variable_link_operand(variable));
								}
								else
								{
									add_expression_node(&new_expression.expression_data, OPERAND, operand);
								}
							}
								
							break;
						}

						default:
						{
							add_expression_node(&new_expression.expression_data, OPERAND, operand);
						}
					}
					break;
				}

				case UNARY_OPERATION:
				{
					add_expression_node(&new_expression.expression_data, UNARY_OPERATION, node.data);
					break;
				}

				case BINARY_OPERATION:
				{
					add_expression_node(&new_expression.expression_data, BINARY_OPERATION, node.data);
					break;
				}
			}
		}
		end_for_each_buffer_element

		add_in_buffer_end(&parser->current_type.expressions, Expression, new_expression);
	end_for_each_buffer_element
}


Boolean parse_type(Bit16* type_name, Buffer* types)
{
	File file = open_file(type_name);
	if(!file)
		return 0;

	Parser parser;
	initialize_file_reader_from_file(&parser.reader, file);
	initialize_buffer(&parser.token, 20);
	initialize_buffer(&parser.expressions, 20);
	parser.has_error = 0;
	parser.types = types;
	parser.current_expression_index = 0;

	//Type type;
	initialize_buffer(&parser.current_type.name, 10);
	while(*type_name)
	{
		add_in_buffer_end(&parser.current_type.name, Bit16, *type_name);
		++type_name;
	}
	initialize_buffer(&parser.current_type.expressions, 100);
	initialize_buffer(&parser.current_type.labels, 100);
	initialize_buffer(&parser.current_type.variables, 100);

	while(!end_of_reader(&parser.reader))
	{
		Expression expression;
		if(!read_expression(&expression.expression_data, &parser))
		{
			parser.has_error = 1;
			break;
		}

		add_in_buffer_end(&parser.expressions, Expression, expression);
		++parser.current_expression_index;
	}

	calculate_variables(&parser);
	calculate_links(&parser);

	deinitialize_reader(&parser.reader);
	add_in_buffer_end(parser.types, Type, parser.current_type);

	return !parser.has_error;
}