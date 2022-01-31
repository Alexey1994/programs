#include <reader.c>


typedef enum
{
	EOF_TOKEN = 256,
	EQUAL_TOKEN,

}
C_Token;


typedef struct
{
	Reader* reader;

}
C_Context;


C_Token read_next_C_token(C_Context* context, Byte* metadata)
{
	Byte character;

	character = get_reader_byte(context->reader);

	switch(character)
	{
		case '0'...'9':
			break;

		case 'a'...'z':
		case 'A'...'Z':
			
			break;
	}

	print(&character);
}


void parse_C(C_Context* context, Reader* reader)
{
	context->reader = reader;

	read_next_C_token(context, 0);
}