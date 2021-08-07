#ifndef LOBJECT_INCLUDED
#define LOBJECT_INCLUDED


//mark for closures active in the stack
#define LUA_TMARK	6

// tags for values visible from Lua == first user-created tag
#define NUM_TAGS	6

// check whether `t' is a mark
#define is_T_MARK(t) ((t) == LUA_TMARK)


typedef union {
	struct TString*  ts; // LUA_TSTRING, LUA_TUSERDATA
	struct Closure*  cl; // LUA_TFUNCTION
	struct Hash*     a;  // LUA_TTABLE
	struct CallInfo* i;  // LUA_TLMARK
	double           n;  // LUA_TNUMBER
}
Value;


// Macros to access values
#define ttype(o)        ((o)->ttype)
#define nvalue(o)       ((o)->value.n)
#define tsvalue(o)      ((o)->value.ts)
#define clvalue(o)      ((o)->value.cl)
#define hvalue(o)       ((o)->value.a)
#define infovalue(o)    ((o)->value.i)
#define svalue(o)       (tsvalue(o)->str)


typedef struct TObject
{
	int   ttype;
	Value value;
}
TObject;


#define TSPACK ((int)sizeof(int))

typedef struct TString
{
	union {
		struct {
			unsigned long hash;
			int constindex;  // hint to reuse constants
		} s; // for strings
		struct {
			int tag;
			void *value;
		} d; // for userdata
	} u;
	size_t len;
	struct TString* nexthash; // chain for hash table
	int    marked;
	char   str[TSPACK]; // variable length string!! must be the last field!
}
TString;


typedef struct
{
	Number*          knum;  // Number numbers used by the function
	int              nknum;  // size of `knum'
	struct TString** kstr;  // strings used by the function
	int              nkstr;  // size of `kstr'
	struct Proto**   kproto;  // functions defined inside the function
	int              nkproto;  // size of `kproto'
	Instruction*     code;
	int              ncode;  // size of `code'; when 0 means an incomplete `Proto'
	short            numparams;
	short            is_vararg;
	short            maxstacksize;
	short            marked;
	struct Proto*    next;
	// debug information
	int*             lineinfo; // map from opcodes to source lines
	int              nlineinfo; // size of `lineinfo'
	int              nlocvars;
	struct LocVar*   locvars;  // information about local variables
	int              lineDefined;
	TString*         source;
}
Proto;


typedef struct LocVar
{
	TString *varname;
	int startpc;  // first point where variable is active
	int endpc;    // first point where variable is dead
}
LocVar;


typedef struct Closure
{
	union {
		lua_CFunction c;  // C functions
		struct Proto* l;  // Lua functions
	} f;
	struct Closure* next;
	struct Closure* mark;  // marked closures (point to itself when not marked)
	short           isC;  // 0 for Lua functions, 1 for C functions
	short           nupvalues;
	TObject         upvalue[1];
}
Closure;


#define iscfunction(o) (ttype(o) == LUA_TFUNCTION && clvalue(o)->isC)


typedef struct
{
	TObject      key;
	TObject      val;
	struct Node* next;  // for chaining
}
Node;

typedef struct Hash
{
	Node*        node;
	int          htag;
	int          size;
	Node*        firstfree;  // this position is free; all positions after it are full
	struct Hash* next;
	struct Hash* mark;  // marked tables (point to itself when not marked)
}
Hash;


#define ismarked(x) ((x)->mark != (x))


typedef struct
{
	struct Closure*     func;  // function being called
	const Instruction** pc;  // current pc of called function
	int                 lastpc;  // last pc traced
	int                 line;  // current line
	int                 refi;  // current index in `lineinfo'
}
CallInfo;


#define luaO_typename(o) (luaO_typenames[ttype(o)])


TObject luaO_nilobject = {LUA_TNIL, {0}};

Byte** luaO_typenames[] = {
	"userdata",
	"nil",
	"number",
	"string",
	"table",
	"function"
};


int luaO_equalObj(TObject *t1, TObject *t2)
{
	if (ttype(t1) != ttype(t2))
		return 0;

	switch (ttype(t1))
	{
		case LUA_TNUMBER:
			return nvalue(t1) == nvalue(t2);
		case LUA_TSTRING: case LUA_TUSERDATA:
			return tsvalue(t1) == tsvalue(t2);
		case LUA_TTABLE: 
			return hvalue(t1) == hvalue(t2);
		case LUA_TFUNCTION:
			return clvalue(t1) == clvalue(t2);
		default:
			return 1;
	}
}


char* luaO_openspace(lua_State* L, Number n) {
	if(n > L->Mbuffsize)
	{
		luaM_reallocvector(L, L->Mbuffer, n, char);
		L->nblocks += (n - L->Mbuffsize) * sizeof(char);
		L->Mbuffsize = n;
	}

	return L->Mbuffer;
}


int luaO_str2d(Byte* s, Number* result)
{
	char*          endptr;
	Integer_Number res;

	res = lua_str2number(s, &endptr);

	if(endptr == s)
		return 0;

	while(isspace((unsigned char)*endptr))
		++endptr;

	if(*endptr != '\0')
		return 0;

	*result = res;

	return 1;
}


// maximum length of a string format for `luaO_verror'
#define MAX_VERROR	280

// this function needs to handle only '%d' and '%.XXs' formats
void luaO_verror(lua_State* L, const char* fmt, ...)
{
	va_list argp;
	char    buff[MAX_VERROR];  // to hold formatted message

	va_start(argp, fmt);
	vsprintf(buff, fmt, argp);
	va_end(argp);
	lua_error(L, buff);
}


void luaO_chunkid(char* out, const char* source, int bufflen) {
	if(*source == '=')
	{
		strncpy(out, source+1, bufflen);  // remove first char
		out[bufflen-1] = '\0';  // ensures null termination
	}
	else
	{
		if(*source == '@')
		{
			int l;
			
			++source;  // skip the `@'
			bufflen -= sizeof("file `...%s'");
			l = strlen(source);
			
			if(l > bufflen)
			{
				source += (l-bufflen);  // get last part of file name
				sprintf(out, "file `...%.99s'", source);
			}
			else
				sprintf(out, "file `%.99s'", source);
		}
		else
		{
			int len = strcspn(source, "\n");  // stop at first newline
			bufflen -= sizeof("string \"%.*s...\"");

			if (len > bufflen)
				len = bufflen;

			if (source[len] != '\0')
			{  // must truncate?
				strcpy(out, "string \"");
				out += strlen(out);
				strncpy(out, source, len);
				strcpy(out+len, "...\"");
			}
			else
				sprintf(out, "string \"%.99s\"", source);
		}
	}
}



#endif//LOBJECT_INCLUDED