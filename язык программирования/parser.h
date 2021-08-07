#ifndef PARSER_H_INCLUDED
#define PARSER_H_INCLUDED


#include <expression-parser/expression-parser.h>
#include "operand.h"


/*
typedef struct
{
	Operand_Type type;
	N_32         address;
	Buffer*      expressions;
	Buffer       local_variables;
	Operand_Type type2;
}
Function_Body_Operand;
*/

typedef struct
{
	Operand_Type type;
	N_32         address;
	Buffer       name;
	Operand_Type type2;
}
Call_Operand;



typedef struct
{
	Buffer expression_data;
	Byte*  metadata;
}
Expression;


typedef struct
{
	Operand_Type type;
	Buffer       name;
	Byte*        data;
}
Type_Node;

typedef struct
{
	Byte*   type;
	Buffer  name;
	Buffer  variables;
	Buffer  expressions;
	Buffer  labels;
}
Type;


typedef struct
{
	Reader  reader;
	Buffer  token;
	Buffer  expressions;
	N_32    current_expression_index;
	//Buffer  labels;
	//Buffer  variables;
	//Buffer  types;
	//Buffer  result_expressions;
	Type    current_type;
	Buffer* types;
	Boolean has_error;
}
Parser;

/*
//Boolean           parse               (Parser* parser, File file, Buffer* types);
Variable_Operand* find_variable       (Parser* parser, Buffer* name);
Label_Operand*    find_label          (Parser* parser, Buffer* name);
Type_Node*        get_type_node       (Parser* parser, N_32 type_index, Buffer* node_name);
N_32              get_type_node_index (Parser* parser, N_32 type_index, Buffer* node_name);
*/

Boolean parse_type(Bit16* type_name, Buffer* types);

#include "parser.c"
#endif //PARSER_H_INCLUDED