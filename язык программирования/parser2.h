#ifndef PARSER_H_INCLUDED
#define PARSER_H_INCLUDED


#include <expression-parser/expression-parser.h>


typedef struct
{
	Buffer  name;
	Buffer  variables;
	Buffer  expressions;
	Buffer  labels;
}
Type;


Type* parse_type(Buffer* type_name, Buffer* types);


#include "parser2.c"
#endif //PARSER_H_INCLUDED