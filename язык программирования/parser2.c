#include <file/file.h>
#include "operand.h"
#include "executor.h"


typedef struct
{
	Reader  reader;
	Buffer  token;
	Buffer  expressions;
	N_32    current_expression_index;
	Type*   current_type;
	Buffer* types;
	Boolean has_error;
}
Parser;


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


static N_32 read_next_UTF_8_character_from_string(N_8** string)
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


Z_32 compare_token(Buffer* token, N_8* string)
{
	N_16* token_data;
	N_32  token_length;
	Z_32  result;

	token_data = token->data;
	token_length = token->length / sizeof(Bit16);

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


void print_token(Buffer* token)
{
	for_each_buffer_element(token, N_16, character)
		printf("%c", character);
	end_for_each_buffer_element
}


Z_32 compare_UTF8_string(Buffer* string1, Buffer* string2)
{
	N_16* string1_data;
	N_32  string1_length;
	N_16* string2_data;
	N_32  string2_length;
	Z_32  result;

	string1_data = string1->data;
	string1_length = string1->length / sizeof(Bit16);
	string2_data = string2->data;
	string2_length = string2->length / sizeof(Bit16);

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


Boolean read_expression(Buffer* expression, Parser* parser);


Label_Operand* find_label(Parser* parser, Buffer* name)
{
	for_each_buffer_element(&parser->current_type->labels, Operand*, operand)
		Label_Operand* label = &operand->data;

		if(!compare_UTF8_string(&label->name, name))
			return label;
	end_for_each_buffer_element

	return 0;
}


Variable_Operand* find_variable(Parser* parser, Buffer* name)
{
	for_each_buffer_element(&parser->current_type->variables, Operand*, operand)
		Variable_Operand *variable = &operand->data;

		if(!compare_UTF8_string(&variable->name, name))
			return variable;
	end_for_each_buffer_element

	return 0;
}


Variable_Operand* add_variable(Parser* parser, Buffer* variable_name, N_32 variable_type_index)
{
	Variable_Operand* variable;

	if(find_variable(parser, variable_name))
	{
		printf("variable ");
		print_token(variable_name);
		printf(" was defined\n");
		return 0;
	}

	variable = create_variable_operand(variable_name, parser->current_type->variables.length / sizeof(Variable_Operand*), variable_type_index);
	add_in_buffer_end(&parser->current_type->variables, Operand*, variable);

	return variable;
}


Type* find_type(Buffer* types, Buffer* type_name)
{
	for_each_buffer_element(types, Type*, type)
		if(!compare_UTF8_string(&type->name, type_name))
			return type;
	end_for_each_buffer_element

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

		if(end_of_reader_data)
			operand = create_name_operand(&parser->token);
		else
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
					add_in_buffer_end(&parser->current_type->labels, Label_Operand*, operand);
				}
			}
			else if(character == '[')
			{
				read_UTF_8_character(&parser->reader);

				Type* call_type = parse_type(&parser->token, parser->types);
				if(!call_type)
					goto error;

				Call_Operand* call_operand = create_call_operand(call_type);

				while(!end_of_reader(&parser->reader) && get_reader_byte(&parser->reader) != ']')
				{
					Buffer expression;

					initialize_buffer(&expression, 100);
					
					if(read_expression(&expression, parser))
					{
						add_in_buffer_end(&call_operand->expressions, Buffer, expression);
					}
					else
					{
						deinitialize_buffer(&expression);
						// free
						goto error;
					}
				}

				if(end_of_reader(&parser->reader))
				{
					printf("expected ]\n");
					// free
					goto error;
				}
				read_UTF_8_character(&parser->reader);

				operand = call_operand;
			}
			else
				operand = create_name_operand(&parser->token);
		}
	}
	else
	{
		operand_type = get_reader_byte(&parser->reader);

		if(is_number_character(operand_type))
			operand = create_number_operand(read_N_32(&parser->reader));
		else
			printf("undefined character: %c [character code: %d]\n", operand_type, operand_type);
	}

	read_spaces(&parser->reader);

	return operand;

error:
	return 0;
}


static Boolean read_if_operation(Reader* reader, Byte* operation_name)
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
	}
	return 0;
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
			if(read_if_operation(&parser->reader, "is"))
				return 'v';

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

	if(operation == 'v' && left_operand->type == NAME_OPERAND && right_operand->type == NAME_OPERAND)
	{
		Name_Operand* variable_name_operand = left_operand->data;
		Name_Operand* type_name_operand = right_operand->data;
		Type* type;

		if(!compare_token(&type_name_operand->name, "Number"))
			type = 0;
		else
			type = parse_type(&type_name_operand->name, parser->types);

		if(!add_variable(parser, &variable_name_operand->name, type))
			parser->has_error = 1;
	}
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
	Buffer new_expression;

	for_each_buffer_element(&parser->expressions, Buffer, expression)
		initialize_buffer(&new_expression, 100);
		
		for_each_buffer_element(&expression, Expression_Node, node)
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
								add_expression_node(&new_expression, OPERAND, create_label_link_operand(label));	
							}
							else
							{
								Variable_Operand* variable = find_variable(parser, &name_operand->name);

								if(variable)
								{
									add_expression_node(&new_expression, OPERAND, create_variable_link_operand(variable));
								}
								else
								{
									add_expression_node(&new_expression, OPERAND, operand);
								}
							}
								
							break;
						}

						default:
						{
							add_expression_node(&new_expression, OPERAND, operand);
						}
					}
					break;
				}

				case UNARY_OPERATION:
				{
					add_expression_node(&new_expression, UNARY_OPERATION, node.data);
					break;
				}

				case BINARY_OPERATION:
				{
					add_expression_node(&new_expression, BINARY_OPERATION, node.data);
					break;
				}
			}
		}
		end_for_each_buffer_element

		add_in_buffer_end(&parser->current_type->expressions, Buffer, new_expression);
	end_for_each_buffer_element
}


Type* parse_type(Buffer* type_name, Buffer* types)
{
	Type* current_type;

	current_type = find_type(types, type_name);
	if(current_type)
		return current_type;

	File file;

	BUFFER(null_terminated_type_name, type_name->length + 2)
		add_bytes_in_buffer_end(&null_terminated_type_name, type_name->data, type_name->length);
		add_bytes_in_buffer_end(&null_terminated_type_name, "\0\0", 2);
		file = open_file(null_terminated_type_name.data);
	END_BUFFER(null_terminated_type_name)

	if(!file)
		return 0;

	Parser parser;
	initialize_file_reader_from_file(&parser.reader, file);
	initialize_buffer(&parser.token, 20);
	initialize_buffer(&parser.expressions, 20);
	parser.has_error = 0;
	parser.types = types;
	parser.current_expression_index = 0;

	current_type = allocate_memory(sizeof(Type));
	initialize_buffer(&current_type->name, type_name->length);
	add_bytes_in_buffer_end(&current_type->name, type_name->data, type_name->length);
	initialize_buffer(&current_type->expressions, 100);
	initialize_buffer(&current_type->labels, 100);
	initialize_buffer(&current_type->variables, 100);

	add_in_buffer_end(parser.types, Type*, current_type);
	parser.current_type = current_type;

	while(!end_of_reader(&parser.reader))
	{
		Buffer expression;
		if(!read_expression(&expression, &parser))
		{
			parser.has_error = 1;
			break;
		}

		add_in_buffer_end(&parser.expressions, Buffer, expression);
		++parser.current_expression_index;
	}

	calculate_variables(&parser);
	calculate_links(&parser);

	deinitialize_reader(&parser.reader);

	if(parser.has_error)
		return 0;

	return current_type;
}