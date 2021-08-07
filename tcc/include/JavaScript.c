#ifndef JAVASCRIPT_INCLUDED
#define JAVASCRIPT_INCLUDED


#include <reader.c>
#include <string.c>
#include <expression-parser.c>
#include <data-structures/stack.c>


typedef struct
{
	Stack token;
	Reader* reader;
}
JavaScript_Parser;


void initialize_JavaScript_parser(JavaScript_Parser* parser)
{
	initialize_stack(&parser->token, 128);
}


void deinitialize_JavaScript_parser(JavaScript_Parser* parser)
{
	deinitialize_stack(&parser->token);
}


Boolean is_latin_character(Number16 character)
{
	return character >= 'a' && character <= 'z' || character >= 'A' && character <= 'Z';
}


Boolean is_cyrillic_character(Number16 character)
{
	return character >= 0x0410 && character <= 0x044F || character == 0x0451 || character == 0x0401;
}


static Boolean parse_JavaScript_token(JavaScript_Parser* parser)
{
	Number16 character;

	clean_stack(&parser->token);

	if(end_of_reader(parser->reader))
		goto error;

	character = get_reader_UTF_8_character(parser->reader);

	if(!is_latin_character(character) && !is_cyrillic_character(character) && character != '_')
		goto error;

	read_UTF_8_character(parser->reader);
	add_bytes_in_stack(&parser->token, &character, sizeof(Number16));

	while(!end_of_reader(parser->reader))
	{
		character = get_reader_UTF_8_character(parser->reader);

		if(!is_latin_character(character) && !is_cyrillic_character(character) && !is_number_character(character) && character != '_')
			break;

		character = read_UTF_8_character(parser->reader);
		add_bytes_in_stack(&parser->token, &character, sizeof(Number16));
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
		printf("%c", data[i]);
}


Boolean is_JavaScript_token(JavaScript_Parser* parser, Byte* token)
{
	return !compare_long_string_and_UTF_8_string(parser->token.data, parser->token.size / sizeof(Number16), token);
}


static Byte* parse_JavaScript_expression_operand(JavaScript_Parser* parser)
{
	Byte* operand;
	Byte  operand_type;

	operand = 0;

	parse_JavaScript_token(parser);
	read_spaces(parser->reader);

	if(parser->token.size)
	{
		print_token(&parser->token);
	}
	else
	{
		operand_type = get_reader_byte(parser->reader);

		if(is_number_character(operand_type))
		{

		}
		else
			printf("undefined character: %c [character code: %d]\n", operand_type, operand_type);
	}

	read_spaces(parser->reader);

	return 0;
}


typedef enum
{
	JS_UNARY_PLUS,
	JS_UNARY_MINUS,
}
Unary_JavaScript_Expression_Operation;

static Operation unary_JavaScript_expression_operations[] = {
	{JS_UNARY_PLUS,  1},
	{JS_UNARY_MINUS, 1}
};

static Operation* parse_unary_JavaScript_expression_operand(JavaScript_Parser* parser)
{
	return 0;
}


typedef enum
{
	JS_PLUS,
	JS_MINUS,
	JS_MULTIPLY,
	JS_DIVIDE
}
Binary_JavaScript_Expression_Operation;

static Operation binary_JavaScript_expression_operations[] = {
	{JS_PLUS,     2},
	{JS_MINUS,    2},
	{JS_MULTIPLY, 1},
	{JS_DIVIDE,   1},
};

static Operation* parse_binary_JavaScript_expression_operand(JavaScript_Parser* parser)
{
	return 0;
}


static Boolean parse_JavaScript_expression(Stack* expression, JavaScript_Parser* parser)
{
	return parse_expression(
		expression,
		parser->reader,
		parser,

		&parse_JavaScript_expression_operand,
		&parse_unary_JavaScript_expression_operand,
		&parse_binary_JavaScript_expression_operand
	);
}

/*
static Byte* parse_JavaScript_operand(JavaScript_Parser* parser)
{
	Byte* operand;
	Byte  operand_type;

	operand = 0;

	parse_JavaScript_token(parser);
	read_spaces(parser->reader);

	if(parser->token.size)
	{
		print_token(&parser->token);
	}
	else
	{
		operand_type = get_reader_byte(parser->reader);

		if(is_number_character(operand_type))
		{

		}
		else
			printf("undefined character: %c [character code: %d]\n", operand_type, operand_type);
	}

	read_spaces(parser->reader);

	return 0;
}


typedef enum
{
	JS_UNARY_PLUS,
	JS_UNARY_MINUS,
}
Unary_JavaScript_Operation;

static Operation unary_JavaScript_operations[] = {
	{JS_UNARY_PLUS,  1},
	{JS_UNARY_MINUS, 1}
};

static Operation* parse_unary_JavaScript_operand(JavaScript_Parser* parser)
{
	return 0;
}


typedef enum
{
	JS_PLUS,
	JS_MINUS,
	JS_MULTIPLY,
	JS_DIVIDE
}
Binary_JavaScript_Operation;

static Operation binary_JavaScript_operations[] = {
	{JS_PLUS,     2},
	{JS_MINUS,    2},
	{JS_MULTIPLY, 1},
	{JS_DIVIDE,   1},
};

static Operation* parse_binary_JavaScript_operand(JavaScript_Parser* parser)
{
	return 0;
}


static Boolean parse_JavaScript_main_expression(Stack* expression, JavaScript_Parser* parser)
{
	return parse_expression(
		expression,
		parser->reader,
		parser,

		&parse_JavaScript_operand,
		&parse_unary_JavaScript_operand,
		&parse_binary_JavaScript_operand
	);
}*/


void parse_JavaScript(JavaScript_Parser* parser, Reader* reader)
{
	Stack expression;
	Byte  token_type;

	parser->reader = reader;

	parse_JavaScript_token(parser);
	read_spaces(parser->reader);

	if(parser->token.size)
	{
		if(is_JavaScript_token(parser, "var"))
		{
			printf("var\n");
		}
		else if(is_JavaScript_token(parser, "function"))
		{
			printf("function\n");
		}
	}
	else
	{
		token_type = get_reader_byte(parser->reader);

		switch(token_type)
		{
			case '{':
				read_next_byte(parser->reader);
				printf("block\n");
				break;

		}
	}

	//parse_JavaScript_expression(&expression, parser);
}


#endif//JAVASCRIPT_INCLUDED


void main()
{
	JavaScript_Parser parser;
	Reader reader;

	initialize_JavaScript_parser(&parser);
	initialize_null_terminated_bytes_reader(&reader,
		"var a = 1"
	);
	parse_JavaScript(&parser, &reader);
	deinitialize_JavaScript_parser(&parser);
}