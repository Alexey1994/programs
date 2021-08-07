#ifndef INTERPRETER_H_INCLUDED
#define INTERPRETER_H_INCLUDED


#include "parser2.h"


typedef struct
{
	Type*  current_type;
	Buffer execution_stack;
	N_32   base_address;
}
Generator;


void interprete(Type* type);


#include "interpreter2.c"
#endif //INTERPRETER_H_INCLUDED