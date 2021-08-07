#ifndef INTERPRETER_H_INCLUDED
#define INTERPRETER_H_INCLUDED

#define DEBUG

#include <log/log.h>
#include "parser.h"
#include "executor.h"

/*
typedef enum
{

}
Interpreter_Operand_Type;
*/

typedef struct
{
	Parser* parser;
	Buffer  execution_stack;
	N_32    base_address;
}
Generator;


void interprete(Parser* parser);


#include "interpreter.c"
#endif //INTERPRETER_H_INCLUDED