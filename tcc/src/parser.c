static const char tcc_keywords[] = 
#define DEF(id, str) str "\0"
#include "tokens.c"
#undef DEF
;


Number tcc_preprocess(TCC_State* state)
{
    return 0;
}