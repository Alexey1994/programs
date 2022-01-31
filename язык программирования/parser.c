#include <reader.c>
#include <data-structures/stack.c>


typedef enum
{
	EOF_TOKEN = 256,
	EQUAL_TOKEN,
	NOT_EQUAL_TOKEN,
	GREATHER_OR_EQUAL_TOKEN,
	LESSER_OR_EQUAL_TOKEN,
	ARROW_TOKEN,
	CONST_NUMBER_TOKEN,
	CONST_STRING_TOKEN,
	NUMBER_TYPE_TOKEN,
	IF_TOKEN,
	NAME_TOKEN
}
C_Token;

char C_token_names[][16] = {
	"<EOF>",
	"==",
	"!=",
	">=",
	"<=",
	"->",
	"const number",
	"const string",
	"Number",
	"if",
	"name",
};


typedef struct
{
	Stack name;
	Stack inputs;
	Stack outputs;
	Stack body;
}
C_Function;

typedef struct
{
	Reader* reader;
	Number  number;
	Stack   name;
	Stack   string;
	Number  line_number;
	Boolean is_error;

	C_Token token;
}
C_Context;


void print_token(C_Token token)
{
	if(token < 256)
	{
		Byte s[2];
		s[0] = token;
		s[1] = '\0';
		print("s", s);
	}
	else
	{
		print("s", C_token_names[token - 256]);
	}
}


C_Token read_next_C_token(C_Context* context)
{
	Byte  character;

repeat:

	character = get_reader_byte(context->reader);

	if(end_of_reader(context->reader))
		return EOF_TOKEN;

	read_next_byte(context->reader);

	switch(character)
	{
		case '\n':
			++context->line_number;
		case '\r':
		case '\t':
		case ' ':
			goto repeat;

		case '#':
			character = get_reader_byte(context->reader);

			if(character == '#')
			{
				read_next_byte(context->reader);

				for(;;)
				{
					if(character == '#')
					{
						read_next_byte(context->reader);
						character = get_reader_byte(context->reader);

						if(character == '#')
							break;
					}

					if(end_of_reader(context->reader))
						return EOF_TOKEN;

					read_next_byte(context->reader);
					character = get_reader_byte(context->reader);
				}
			}
			else
			{
				for(;;)
				{
					if(character == '\n')
						break;

					if(end_of_reader(context->reader))
						return EOF_TOKEN;

					read_next_byte(context->reader);
					character = get_reader_byte(context->reader);
				}
			}

			goto repeat;

		case '>':
			if(get_reader_byte(context->reader) == '=')
			{
				read_next_byte(context->reader);
				return GREATHER_OR_EQUAL_TOKEN;
			}
			break;

		case '<':
			if(get_reader_byte(context->reader) == '=')
			{
				read_next_byte(context->reader);
				return LESSER_OR_EQUAL_TOKEN;
			}
			break;

		case '!':
			if(get_reader_byte(context->reader) == '=')
			{
				read_next_byte(context->reader);
				return NOT_EQUAL_TOKEN;
			}
			break;

		case '-':
			if(get_reader_byte(context->reader) == '>')
			{
				read_next_byte(context->reader);
				return ARROW_TOKEN;
			}
			break;

		case '0'...'9':

			context->number = character - '0';
			
			for(;;)
			{
				character = get_reader_byte(context->reader);

				if(character < '0' || character > '9')
					break;

				if(end_of_reader(context->reader))
					break;

				context->number *= 10;
				context->number += character - '0';

				read_next_byte(context->reader);
			}

			return CONST_NUMBER_TOKEN;

		case '"':
			initialize_stack(&context->string, 16);

			for(;;)
			{
				character = get_reader_byte(context->reader);
				read_next_byte(context->reader);

				if(character == '"')
					break;

				if(character == '\\')
				{
					character = get_reader_byte(context->reader);
					read_next_byte(context->reader);

					switch(character)
					{
						case '\\':
							add_bytes_in_stack(&context->string, &character, 1);
							break;

						case 'n':
							character = '\n';
							add_bytes_in_stack(&context->string, &character, 1);
							break;

						case 'r':
							character = '\r';
							add_bytes_in_stack(&context->string, &character, 1);
							break;

						case 't':
							character = '\t';
							add_bytes_in_stack(&context->string, &character, 1);
							break;
					}
				}
				else
					add_bytes_in_stack(&context->string, &character, 1);

				if(end_of_reader(context->reader))
				{
					//context->is_error = 1;
					return 0;
				}
			}

			character = '\0';
			add_bytes_in_stack(&context->string, &character, 1);

			return CONST_STRING_TOKEN;

		case 'a'...'z':
		case 'A'...'Z':
		case '_':
			initialize_stack(&context->name, 16);

			while(!end_of_reader(context->reader))
			{
				add_bytes_in_stack(&context->name, &character, 1);
				character = get_reader_byte(context->reader);

				if((character < 'a' || character > 'z') && (character < 'A' || character > 'Z') && character != '_' && (character < '0' || character > '9'))
					break;

				read_next_byte(context->reader);
			}

			character = '\0';
			add_bytes_in_stack(&context->name, &character, 1);

			if(!compare_null_terminated_bytes(context->name.data, "Number"))
				return NUMBER_TYPE_TOKEN;
			else if(!compare_null_terminated_bytes(context->name.data, "if"))
				return IF_TOKEN;

			return NAME_TOKEN;

			//print("s", context->name.data);
			break;
	}

	return character;
}


Boolean required_C_token(C_Context* context, C_Token token)
{
	if(context->is_error)
		return 0;

	if(context->token != token)
	{
		print("ns", context->line_number + 1, ": отсутствует ");
		print_token(token);
		print("s", ", получен ");
		print_token(context->token);
		context->is_error = 1;
		return 0;
	}

	context->token = read_next_C_token(context);

	return 1;
}

Boolean optional_C_token(C_Context* context, C_Token token)
{
	if(context->is_error)
		return 0;

	if(context->token != token)
		return 0;

	context->token = read_next_C_token(context);

	return 1;
}


void expression_C_state(C_Context* context);


void variable_or_function_call_C_state(C_Context* context)
{
	if(optional_C_token(context, '.'))
	{
		required_C_token(context, NAME_TOKEN);
		variable_or_function_call_C_state(context);
	}
	else if(optional_C_token(context, '('))
	{
		if(!optional_C_token(context, ')'))
		{
			do
			{
				expression_C_state(context);
			}
			while(optional_C_token(context, ','));

			required_C_token(context, ')');
		}

		variable_or_function_call_C_state(context);
	}
}


Number get_C_operation_priority(C_Token operation_token)
{
	switch(operation_token)
	{
		case '+':
			return 2;

		case '-':
			return 2;

		case '*':
			return 3;

		case '/':
			return 3;

		case '%':
			return 3;

		case '=':
			return 4;

		case '(':
			return 1;

		case 0:
			return 0;

		default:
			print("неизвестная операция\n");
	}
}


Boolean expression1_C_state(C_Context* context, Boolean require_operand, C_Token* previouse_operation);


Boolean add_C_expression_operation(C_Context* context, C_Token operation, C_Token* previouse_operation)
{
	context->token = read_next_C_token(context);

	if(operation != '(' && get_C_operation_priority(operation) <= get_C_operation_priority(*previouse_operation))
	{
		print_token(*previouse_operation);
		*previouse_operation = 0;
	}

	if(expression1_C_state(context, 1, &operation))
	{
		if(operation == '(')
			return expression1_C_state(context, 0, &operation);

		if(operation)
			print_token(operation);

		return 1;
	}

	if(operation == '(')
	{
		print("ns", context->line_number + 1, ": отсутствует )");
		context->is_error = 1;
	}
	else if(operation)
		print_token(operation);

	return 0;
}


Boolean expression1_C_state(C_Context* context, Boolean require_operand, C_Token* previouse_operation)
{
	if(context->token == '(')
	{
		if(!require_operand)
			return 0;

		return add_C_expression_operation(context, '(', previouse_operation);
	}
	else
	{
		if(require_operand)
		{
			if(context->token == ')')
			{
				print("ns", context->line_number + 1, ": ) не может быть здесь");
				context->is_error = 1;
			}

			switch(context->token)
			{
				case NAME_TOKEN:
					print("s", "operand ");
					context->token = read_next_C_token(context);
					variable_or_function_call_C_state(context);
					break;

				case CONST_NUMBER_TOKEN:
					print("ns", context->number, " ");
					context->token = read_next_C_token(context);
					break;

				case CONST_STRING_TOKEN:
					print("ss", context->string.data, " ");
					context->token = read_next_C_token(context);
					break;

				default:
					print("ns", context->line_number + 1, ": ожидалось ");
					print_token(NAME_TOKEN);
					print("s", ", ");
					print_token(CONST_NUMBER_TOKEN);
					print("s", ", ");
					print_token(CONST_STRING_TOKEN);
					print("s", ", получен ");
					print_token(context->token);
					context->is_error = 1;
			}
		}

		switch(context->token)
		{
			case '+':
			case '-':
			case '*':
			case '/':
			case '%':
			case '=':
				return add_C_expression_operation(context, context->token, previouse_operation);

			case ')':
				if(*previouse_operation == 0)
				{
					//print("ns", context->line_number + 1, ": отсутствует )");
					//context->is_error = 1;
					return 0;
				}

				context->token = read_next_C_token(context);

				return 1;
		}
	}

	return 0;
}


void expression_C_state(C_Context* context)
{
	C_Token current_operation;

	current_operation = 0;
	//context->token = read_next_C_token(context);
	expression1_C_state(context, 1, &current_operation);
	print("s", "\n");
}


void body_C_state(C_Context* context)
{
	while(!context->is_error && !optional_C_token(context, '}'))
	{
		if(optional_C_token(context, EOF_TOKEN))
		{
			print("ns", context->line_number + 1, ": отсутствует ");
			print_token(EOF_TOKEN);
			context->is_error = 1;
			break;
		}
		else if(optional_C_token(context, IF_TOKEN))
		{
			expression_C_state(context);
			required_C_token(context, '{');
			body_C_state(context);
		}
		else
		{
			expression_C_state(context);
		}
	}
}


void function_C_state(C_Context* context)
{
	required_C_token(context, NAME_TOKEN);
	print("ss", context->name.data, "\n");

	required_C_token(context, '(');

	do
	{
		if(optional_C_token(context, NUMBER_TYPE_TOKEN))
		{
			required_C_token(context, NAME_TOKEN);
		}
		else if(optional_C_token(context, NAME_TOKEN))
		{
			required_C_token(context, NAME_TOKEN);
		}
	}
	while(optional_C_token(context, ','));

	required_C_token(context, ')');

	required_C_token(context, ARROW_TOKEN);

	required_C_token(context, '(');

	do
	{
		if(optional_C_token(context, NUMBER_TYPE_TOKEN))
		{
			required_C_token(context, NAME_TOKEN);
		}
		else if(optional_C_token(context, NAME_TOKEN))
		{
			required_C_token(context, NAME_TOKEN);
		}
	}
	while(optional_C_token(context, ','));

	required_C_token(context, ')');

	required_C_token(context, '{');

	body_C_state(context);
}


void parse_C(C_Context* context, Reader* reader)
{
	context->reader = reader;
	context->line_number = 0;
	context->is_error = 0;

	context->token = read_next_C_token(context);

	while(!context->is_error && !optional_C_token(context, EOF_TOKEN))
	{
		function_C_state(context);
	}
	
}