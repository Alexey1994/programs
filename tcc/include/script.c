/*
Number a
Number b

c = a, b
print(c(0), c(1))



Число а
Число б

в = а, б
напечатать(в(0), в(1))



Данные8 массив
Число   количество_байт_в_массиве

следующий_элемент: (количество_байт_в_массиве <= 0) => конец
                   напечатать <- массив.байт
                   количество_байт_в_массиве = количество_байт_в_массиве - 1
                   массив = массив + 1
                   следующий_элемент
конец:
*/


#ifndef SCRIPT_INCLUDED
#define SCRIPT_INCLUDED


#include <reader2.c>
#include <string.c>
#include <expression-parser.c>
#include <data-structures/stack.c>


typedef enum
{
	SCRIPT_LINK_OPERAND,
	SCRIPT_LABEL_OPERAND,
	SCRIPT_CONST_NUMBER_OPERAND,
	SCRIPT_DATA_OPERAND
}
Script_Operand_Type;

typedef struct
{
	Script_Operand_Type type;
	Byte                data[];
}
Script_Operand;

typedef struct
{
	Script_Operand_Type  type;
	Stack                name;
	struct Script_Field* field;
	struct Script_Label_Operand* label;
}
Script_Link_Operand;

typedef struct
{
	Script_Operand_Type  type;
	Stack                name;
}
Script_Label_Operand;

typedef struct
{
	Script_Operand_Type type;
	Number              value;
}
Script_Const_Number_Operand;

typedef struct
{
	Script_Operand_Type type;
	Stack               expression;
}
Script_Data_Operand;


Script_Operand* create_Script_Link_Operand(Stack* name)
{
	Script_Link_Operand* operand;

	operand = allocate_memory(sizeof(Script_Link_Operand));
	operand->type  = SCRIPT_LINK_OPERAND;
	operand->field = 0;
	operand->label = 0;
	initialize_stack(&operand->name, name->size);
	add_bytes_in_stack(&operand->name, name->data, name->size);

	return operand;
}


Script_Operand* create_Script_Label_Operand(Stack* name)
{
	Script_Label_Operand* operand;

	operand = allocate_memory(sizeof(Script_Label_Operand));
	operand->type  = SCRIPT_LABEL_OPERAND;
	initialize_stack(&operand->name, name->size);
	add_bytes_in_stack(&operand->name, name->data, name->size);

	return operand;
}


Script_Operand* create_Script_Const_Number_Operand(Number value)
{
	Script_Const_Number_Operand* operand;

	operand = allocate_memory(sizeof(Script_Const_Number_Operand));
	operand->type = SCRIPT_CONST_NUMBER_OPERAND;
	operand->value = value;

	return operand;
}


typedef struct
{
	Script_Link_Operand* link;
	Number               index;
}
Script_Field;


typedef struct
{
	Stack name;
	Stack fields;
	Stack labels;
	Stack expressions;
}
Script_Type;


typedef struct
{
	Stack        token;
	Reader*      reader;
	Stack        types;
	Script_Type* current_type;

	Script_Link_Operand* field_definition_link;
}
Script_Parser;


void initialize_Script_parser(Script_Parser* parser)
{
	initialize_stack(&parser->token, 128);
	initialize_stack(&parser->types, 128);
	parser->field_definition_link = 0;
}


void deinitialize_Script_parser(Script_Parser* parser)
{
	deinitialize_stack(&parser->token);
	deinitialize_stack(&parser->types);
}


Boolean is_latin_character(Number16 character)
{
	return character >= 'a' && character <= 'z' || character >= 'A' && character <= 'Z';
}


Boolean is_cyrillic_character(Number16 character)
{
	return character >= 0x0410 && character <= 0x044F || character == 0x0451 || character == 0x0401;
}


static Boolean parse_Script_token(Script_Parser* parser)
{
	Number16 character;

	clean_stack(&parser->token);

	if(end_of_reader(parser->reader))
		goto error;

	character = get_reader_UTF_8_character(parser->reader);

	if(!is_latin_character(character) && !is_cyrillic_character(character) && character != '_')
		goto error;

	read_UTF_8_character(parser->reader);
	add_bytes_in_stack(&parser->token, &character, sizeof(character));

	while(!end_of_reader(parser->reader))
	{
		character = get_reader_UTF_8_character(parser->reader);

		if(!is_latin_character(character) && !is_cyrillic_character(character) && !is_number_character(character) && character != '_')
			break;

		character = read_UTF_8_character(parser->reader);
		add_bytes_in_stack(&parser->token, &character, sizeof(character));
	}

	return 1;

error:
	return 0;
}


void print_token(Stack* token)
{
	Number    size;
	Number16* data;
	Number    i;

	size = token->size / sizeof(Number16);
	data = token->data;
	for(i = 0; i < size; ++i)
		write_character_in_console(get_default_console_writer(), data[i]);
}


Boolean is_Script_token(Script_Parser* parser, Byte* token)
{
	return !compare_long_string_and_UTF_8_string(parser->token.data, parser->token.size / sizeof(Number16), token);
}


static Boolean parse_Script_expression(Stack* expression, Script_Parser* parser);

Script_Operand* create_Script_Data_Operand(Script_Parser* parser)
{
	Script_Data_Operand* operand;

	operand = allocate_memory(sizeof(Script_Data_Operand));
	operand->type = SCRIPT_DATA_OPERAND;

	if(!parse_Script_expression(&operand->expression, parser))
		goto error;

	if(get_reader_byte(parser->reader) == ']')
		read_next_byte(parser->reader);
	else
		goto error;

	return operand;

error:
	free_memory(operand);
	return 0;
}


static Byte* parse_Script_expression_operand(Script_Parser* parser)
{
	Script_Operand* operand;
	Byte            operand_type;

	operand = 0;

	parse_Script_token(parser);
	read_spaces(parser->reader);

	if(parser->token.size)
	{
		read_spaces(parser->reader);

		if(get_reader_byte(parser->reader) == '[')
			printf("call\n");
		//else

		if(get_reader_byte(parser->reader) == ':')
		{
			read_next_byte(parser->reader);
			operand = create_Script_Label_Operand(&parser->token);
			add_bytes_in_stack(&parser->current_type->labels, &operand, sizeof(operand));
		}
		else
		{
			operand = create_Script_Link_Operand(&parser->token);
		}
	}
	else
	{
		operand_type = get_reader_byte(parser->reader);

		if(operand_type == '[')
		{
			read_next_byte(parser->reader);
			operand = create_Script_Data_Operand(parser);
		}
		else if(is_number_character(operand_type))
			operand = create_Script_Const_Number_Operand(read_Number(parser->reader));
		else
			printf("undefined character: %c [character code: %d]\n", operand_type, operand_type);
	}

	read_spaces(parser->reader);

	return operand;
}


typedef enum
{
	SCRIPT_UNARY_MINUS,
	SCRIPT_NUMBER_TYPE,
	SCRIPT_DATA_START,
	SCRIPT_PRINT,
}
Unary_Script_Expression_Operation;

static Operation unary_Script_expression_operations[] = {
	{SCRIPT_UNARY_MINUS, 1},
	{SCRIPT_NUMBER_TYPE, 1},
	{SCRIPT_DATA_START,  5},
	{SCRIPT_PRINT,       10},
};

static Operation* parse_unary_Script_expression_operation(Script_Parser* parser)
{
	Number32 operation_type;

	operation_type = get_reader_UTF_8_character(parser->reader);

	switch(operation_type)
	{
		case '-':
			read_next_byte(parser->reader);
			return SCRIPT_UNARY_MINUS + unary_Script_expression_operations;

		case 'N':
		case 1063:
			if(read_if(parser->reader, "Number") || read_if(parser->reader, "Число"))
				return SCRIPT_NUMBER_TYPE + unary_Script_expression_operations;

		//case '[':
		//	read_next_byte(parser->reader);
		//	return SCRIPT_DATA_START + unary_Script_expression_operations;

		case 1085:
			if(read_if(parser->reader, "написать"))
				return SCRIPT_PRINT + unary_Script_expression_operations;
	}

	return 0;
}


typedef enum
{
	SCRIPT_DATA_END,
}
Postfix_Unary_Script_Expression_Operation;

static Operation postfix_unary_Script_expression_operations[] = {
	{SCRIPT_DATA_END,  6},
};

static Operation* parse_postfix_unary_Script_expression_operation(Script_Parser* parser)
{
	Number32 operation_type;

	operation_type = get_reader_UTF_8_character(parser->reader);

	switch(operation_type)
	{
		//case ']':
		//	read_next_byte(parser->reader);
		//	return SCRIPT_DATA_END + postfix_unary_Script_expression_operations;
	}

	return 0;
}


typedef enum
{
	//SCRIPT_CALL,
	SCRIPT_IF,
	SCRIPT_ASSIGNMENT,
	SCRIPT_COMMA,
	SCRIPT_PLUS,
	SCRIPT_MINUS,
	SCRIPT_MULTIPLY,
	SCRIPT_DIVIDE,
	SCRIPT_EQUAL,
	SCRIPT_LESSER,
	SCRIPT_LESSER_OR_EQUAL,
	SCRIPT_GREATHER,
	SCRIPT_GREATHER_OR_EQUAL,
	SCRIPT_POINT,


}
Binary_Script_Expression_Operation;

static Operation binary_Script_expression_operations[] = {
	//{SCRIPT_CALL,              10},
	{SCRIPT_IF,                9},
	{SCRIPT_ASSIGNMENT,        8},
	{SCRIPT_COMMA,             7},
	{SCRIPT_PLUS,              3},
	{SCRIPT_MINUS,             3},
	{SCRIPT_MULTIPLY,          2},
	{SCRIPT_DIVIDE,            2},
	{SCRIPT_EQUAL,             2},
	{SCRIPT_LESSER,            2},
	{SCRIPT_LESSER_OR_EQUAL,   2},
	{SCRIPT_GREATHER,          2},
	{SCRIPT_GREATHER_OR_EQUAL, 2},
	{SCRIPT_POINT,             1},
};

static Operation* parse_binary_Script_expression_operation(Script_Parser* parser)
{
	Byte   character;
	Number operation_type;

	character = get_reader_byte(parser->reader);

	switch(character)
	{
		case '=':
			read_next_byte(parser->reader);
			character = get_reader_byte(parser->reader);

			if(character == '>')
			{
				read_next_byte(parser->reader);
				return SCRIPT_IF + binary_Script_expression_operations;
			}
			else if(character == '=')
			{
				read_next_byte(parser->reader);
				return SCRIPT_EQUAL + binary_Script_expression_operations;
			}
			else
				return SCRIPT_ASSIGNMENT + binary_Script_expression_operations;

		case '<':
			read_next_byte(parser->reader);
			character = get_reader_byte(parser->reader);

			/*if(character == '-')
			{
				read_next_byte(parser->reader);
				return SCRIPT_CALL + binary_Script_expression_operations;
			}
			else */if(character == '=')
			{
				read_next_byte(parser->reader);
				return SCRIPT_LESSER_OR_EQUAL + binary_Script_expression_operations;
			}
			else
				return SCRIPT_LESSER + binary_Script_expression_operations;

		case '>':
			read_next_byte(parser->reader);
			character = get_reader_byte(parser->reader);

			if(character == '=')
			{
				read_next_byte(parser->reader);
				return SCRIPT_GREATHER_OR_EQUAL + binary_Script_expression_operations;
			}
			else
				return SCRIPT_GREATHER + binary_Script_expression_operations;

		case ',':
			read_next_byte(parser->reader);
			return SCRIPT_COMMA + binary_Script_expression_operations;

		case '.':
			read_next_byte(parser->reader);
			return SCRIPT_POINT + binary_Script_expression_operations;

		case '+':
			read_next_byte(parser->reader);
			return SCRIPT_PLUS + binary_Script_expression_operations;

		case '-':
			read_next_byte(parser->reader);
			character = get_reader_byte(parser->reader);

			return SCRIPT_MINUS + binary_Script_expression_operations;

		case '*':
			read_next_byte(parser->reader);
			return SCRIPT_MULTIPLY + binary_Script_expression_operations;

		case '/':
			read_next_byte(parser->reader);
			return SCRIPT_DIVIDE + binary_Script_expression_operations;
	}

	return 0;
}


static Boolean parse_Script_expression(Stack* expression, Script_Parser* parser)
{
	return parse_expression(
		expression,
		parser->reader,
		parser,

		&parse_Script_expression_operand,
		&parse_unary_Script_expression_operation,
		&parse_postfix_unary_Script_expression_operation,
		&parse_binary_Script_expression_operation
	);
}


Boolean parse_Script_type(Script_Parser* parser, Reader* reader)
{
	parser->current_type = allocate_bytes_in_stack(&parser->types, sizeof(*parser->current_type));
	initialize_stack(&parser->current_type->name, 128);
	initialize_stack(&parser->current_type->fields, 128);
	initialize_stack(&parser->current_type->labels, 128);
	initialize_stack(&parser->current_type->expressions, 128);

	parser->reader = reader;
	while(!end_of_reader(reader))
	{
		if(!parse_Script_expression(allocate_bytes_in_stack(&parser->current_type->expressions, sizeof(Stack)), parser))
			goto error;
	}

	return 1;

error:
	printf("error\n");
	return 0;
}

///////////////////////////////////////////////////////////////////////////////


Script_Field* find_Script_field(Script_Type* type, Stack* name)
{
	Number        i;
	Script_Field* current_field;

	for(i = 0; i < type->fields.size; i += sizeof(Script_Field))
	{
		current_field = type->fields.data + i;

		if(!compare_bytes(current_field->link->name.data, current_field->link->name.size, name->data, name->size))
			return current_field;
	}

	return 0;
}


Script_Label_Operand* find_Script_label(Script_Type* type, Stack* name)
{
	Number                i;
	Script_Label_Operand* current_label;

	for(i = 0; i < type->labels.size; i += sizeof(Script_Label_Operand*))
	{
		current_label = *(Script_Label_Operand**)(type->labels.data + i);

		if(!compare_bytes(current_label->name.data, current_label->name.size, name->data, name->size))
			return current_label;
	}

	return 0;
}


void calculate_Script_type_fields_operand(Script_Parser* parser, Script_Operand* operand)
{

}


Script_Operand* calculate_Script_type_fields_unary_operation(Script_Parser* parser, Operation* operation, Script_Operand* operand)
{
	switch(operation->data)
	{
		case SCRIPT_NUMBER_TYPE:
			if(operand->type == SCRIPT_LINK_OPERAND)
			{
				parser->field_definition_link = operand;
				return &parser->field_definition_link;
			}
	}

	return 0;
}


Script_Operand* calculate_Script_type_fields_postfix_unary_operation(Script_Parser* parser, Operation* operation, Script_Operand* operand)
{
	return 0;
}


Script_Operand* calculate_Script_type_fields_binary_operation(Script_Parser* parser, Operation* operation, Script_Operand* operand1, Script_Operand* operand2)
{
	return 0;
}


void calculate_Script_type_fields(Script_Parser* parser, Script_Type* type)
{
	Number          i;
	Stack*          expression;
	Script_Operand* result;
	Script_Field*   field;

	for(i = 0; i < type->expressions.size; i += sizeof(Stack))
	{
		expression = type->expressions.data + i;

		parser->field_definition_link = 0;
		
		result = execute_expression(
			expression,
			parser,

			&calculate_Script_type_fields_operand,
			&calculate_Script_type_fields_unary_operation,
			&calculate_Script_type_fields_postfix_unary_operation,
			&calculate_Script_type_fields_binary_operation
		);

		if(result && result == &parser->field_definition_link)
		{
			if(find_Script_field(type, &parser->field_definition_link->name))
			{
				printf("Field ");
				print_token(&parser->field_definition_link->name);
				printf(" was defined\n");
			}
			else
			{
				field = allocate_bytes_in_stack(&type->fields, sizeof(Script_Field));
				field->link = parser->field_definition_link;
				field->index = type->fields.size / sizeof(Script_Field) - 1;
			}
		}
	}
}


void calculate_Script_fields(Script_Parser* parser)
{
	calculate_Script_type_fields(parser, parser->types.data);
}

///////////////////////////////////////////////////////////////////////////////

void calculate_Script_type_links_operand(Script_Parser* parser, Script_Operand* operand)
{
	Script_Link_Operand* link_operand;

	switch(operand->type)
	{
		case SCRIPT_LINK_OPERAND:
			link_operand = operand;
			link_operand->field = find_Script_field(parser->current_type, &link_operand->name);
			link_operand->label = find_Script_label(parser->current_type, &link_operand->name);
			break;
	}
}


void calculate_Script_type_links_unary_operation(Script_Parser* parser, Operation* operation)
{

}


void calculate_Script_type_links_postfix_unary_operation(Script_Parser* parser, Operation* operation)
{

}


void calculate_Script_type_links_binary_operation(Script_Parser* parser, Operation* operation)
{

}


void calculate_Script_type_links(Script_Parser* parser, Script_Type* type)
{
	Number i;
	Stack* expression;

	parser->current_type = type;

	for(i = 0; i < type->expressions.size; i += sizeof(Stack))
	{
		expression = type->expressions.data + i;
		
		print_expression_in_postfix_notation(
			expression,
			parser,

			&calculate_Script_type_links_operand,
			&calculate_Script_type_links_unary_operation,
			&calculate_Script_type_links_postfix_unary_operation,
			&calculate_Script_type_links_binary_operation
		);
	}
}


void calculate_Script_links(Script_Parser* parser)
{
	calculate_Script_type_links(parser, parser->types.data);
}

///////////////////////////////////////////////////////////////////////////////

void execute_Script_expression(Script_Parser* parser, Stack* expression);

void execute_Script_type_operand(Script_Parser* parser, Script_Operand* operand)
{
	Script_Const_Number_Operand* const_number_operand;
	Script_Link_Operand*         link_operand;
	Script_Data_Operand*         data_operand;

	switch(operand->type)
	{
		case SCRIPT_CONST_NUMBER_OPERAND:
			const_number_operand = operand;
			printf("%d ", const_number_operand->value);
			break;

		case SCRIPT_LINK_OPERAND:
			link_operand = operand;
			print_token(&link_operand->name);
			printf(" ");
			break;

		case SCRIPT_DATA_OPERAND:
			data_operand = operand;
			printf("[");
			execute_Script_expression(parser, &data_operand->expression);
			printf("] ");
			break;
	}
}


void execute_Script_type_unary_operation(Script_Parser* parser, Operation* operation)
{
	switch(operation->data)
	{
		case SCRIPT_UNARY_MINUS:
			printf("unary- ");
			break;

		case SCRIPT_NUMBER_TYPE:
			printf("Number ");
			break;

		case SCRIPT_DATA_START:
			printf("[ ");
			break;

		case SCRIPT_PRINT:
			printf("print ");
			break;
	}
}


void execute_Script_type_postfix_unary_operation(Script_Parser* parser, Operation* operation)
{
	switch(operation->data)
	{
		case SCRIPT_DATA_END:
			printf("] ");
			break;
	}
}


void execute_Script_type_binary_operation(Script_Parser* parser, Operation* operation)
{
	switch(operation->data)
	{
		case SCRIPT_IF:
			printf("=> ");
			break;

		case SCRIPT_ASSIGNMENT:
			printf("= ");
			break;

		case SCRIPT_PLUS:
			printf("+ ");
			break;

		case SCRIPT_MINUS:
			printf("- ");
			break;

		case SCRIPT_MULTIPLY:
			printf("* ");
			break;

		case SCRIPT_DIVIDE:
			printf("/ ");
			break;

		/*case SCRIPT_CALL:
			printf("<- ");
			break;*/

		case SCRIPT_POINT:
			printf(". ");
			break;

		case SCRIPT_COMMA:
			printf(", ");
			break;
	}
}


void execute_Script_expression(Script_Parser* parser, Stack* expression)
{
	print_expression_in_postfix_notation(
		expression,
		parser,

		&execute_Script_type_operand,
		&execute_Script_type_unary_operation,
		&execute_Script_type_postfix_unary_operation,
		&execute_Script_type_binary_operation
	);
}


void execute_Script_type(Script_Parser* parser, Script_Type* type)
{
	Number i;
	Stack* expression;

	for(i = 0; i < type->expressions.size; i += sizeof(Stack))
	{
		expression = type->expressions.data + i;
		execute_Script_expression(parser, type->expressions.data + i);
		/*
		print_expression_in_postfix_notation(
			expression,
			parser,

			&execute_Script_type_operand,
			&execute_Script_type_unary_operation,
			&execute_Script_type_postfix_unary_operation,
			&execute_Script_type_binary_operation
		);*/

		printf("\n");
	}
}


void execute_Script(Script_Parser* parser)
{
	execute_Script_type(parser, parser->types.data);
}


#endif//SCRIPT_INCLUDED


#include <reader/bytes.c>


void main()
{
	Script_Parser parser;
	Reader        reader;

	initialize_Script_parser(&parser);
	initialize_null_terminated_bytes_reader(&reader,
		/*"Number a\n"
		"Number b\n"
		
		"la:\n"

		"1 => la\n"

		"1 + 2 - -3\n"
		"a = 1\n"
		"4 + -5 - a\n"

		"point = f(1, 2)\n"
		"x = point.x\n"*/

		//"Add_Point_to_Point <- (Point <- 1, 2), (Point <- 3, 4)\n"
		//"Print <- Factorial <- 1"

		/*
		"a = 1\n"
		"b = 2\n"
		"тут: Напечатать . (a + b)\n"
		"и_тут: Сотворить . (a + b, 1, 2)\n"
		"Напечатать . 123, 345\n"
		"Напечатать . (а.икс), (б.игрек)\n"*/

		//Точка
		//"Число икс\n"
		//"Число игрек\n"

		//"написать икс\n"
		//"написать игрек\n"

		//Решение
		//"а = Точка 2, 3\n"
		//"б = Точка 3, 4\n"
		//"в = (1, Точка)"

		//"a = [1, 2, [3, 4]]"
		//"a = (1 , 2 , (3 , 4) , 5)"
		"a = (1, 2)"
		//"a = [[0], [1 + 2]]"//]\n"
		//"b = [2, 3]\n"
		//"c = dot[a, b]\n"
	);
	parse_Script_type(&parser, &reader);
	calculate_Script_fields(&parser);
	//calculate_Script_links(&parser);
	execute_Script(&parser);
	deinitialize_Script_parser(&parser);
}

/*
Точка а
Точка б
Точка нормаль

нормаль = а.y * б.z - а.z * б.y, а.x * б.z - а.z * б.x, а.x * б.y - а.y * б.x -> Точка
*/