#define size_t int
#define INT_MAX 2147483647
#define ULONG_MAX 0xffffffffUL
#define NULL 0
#define	EXIT_FAILURE 1

typedef char* FILE;


#define	STDIN_FILENO	0
#define	STDOUT_FILENO	1
#define	STDERR_FILENO	2

extern FILE _iob[];

#define stdin	(&_iob[STDIN_FILENO])
#define stderr	(&_iob[STDERR_FILENO])


typedef char *va_list;
#define va_start(ap,last) ap = ((char *)&(last)) + ((sizeof(last)+3)&~3)
#define va_arg(ap,type) (ap += (sizeof(type)+3)&~3, *(type *)(ap - ((sizeof(type)+3)&~3)))
#define va_copy(dest, src) (dest) = (src)
#define va_end(ap)


#define _JBLEN 16
#define _JBTYPE int
typedef _JBTYPE jmp_buf[_JBLEN];
int	_setjmp (jmp_buf);
#define	setjmp(x)	_setjmp(x)
void	longjmp (jmp_buf, int);


double strtod(char* s, char* end);


//#include <types.c>

/*
//////////////////////// lua.h //////////////////


#define LUA_API
#define LUA_ERRORMESSAGE	"_ERRORMESSAGE"
#define LUA_MINSTACK	20
#define LUA_MULTRET	(-1)

typedef enum
{
	LUA_ERRRUN    = 1,
	LUA_ERRFILE   = 2,
	LUA_ERRSYNTAX = 3,
	LUA_ERRMEM    = 4,
	LUA_ERRERR    = 5
}
Lua_Do_Error_Code;

typedef enum
{
	LUA_OK        = 0,
	LUA_YIELD     = 1,
	LUA_ERRRUN    = 2,
	LUA_ERRSYNTAX = 3,
	LUA_ERRMEM    = 4,
	LUA_ERRGCMM   = 5,
	LUA_ERRERR    = 6
}
Lua_Thread_Status;

typedef enum
{
	LUA_TNONE          = -1,
	LUA_TNIL           = 0,
	LUA_TBOOLEAN       = 1,
	LUA_TLIGHTUSERDATA = 2,
	LUA_TNUMBER        = 3,
	LUA_TSTRING        = 4,
	LUA_TTABLE         = 5,
	LUA_TFUNCTION      = 6,
	LUA_TUSERDATA      = 7,
	LUA_TTHREAD        = 8,
	LUA_NUMTAGS        = 9
}
Lua_Basic_Types;


typedef void   (*lua_Hook)(struct lua_State* L, struct Lua_Debug* debug);
typedef int    (*lua_CFunction) (struct lua_State *L);


/////////////////////////////////////////


typedef struct TObject* StkId;  // index to stack elements

typedef struct
{
	// thread-specific state
	StkId               top;         // first free slot in the stack
	StkId               stack;       // stack base
	StkId               stack_last;  // last free slot in the stack
	int                 stacksize;
	StkId               Cbase;       // base for current C function
	struct lua_longjmp* errorJmp;    // current error recover point
	char*               Mbuffer;     // global buffer
	size_t              Mbuffsize;   // size of Mbuffer
	// global state
	struct Proto*       rootproto;   // list of all prototypes
	struct Closure*     rootcl;      // list of all closures
	struct Hash*        roottable;   // list of all tables
	struct stringtable  strt;        // hash table for strings
	struct stringtable  udt;         // hash table for udata
	struct Hash*        gt;          // table for globals
	struct TM*          TMtable;     // table for tag methods
	int                 last_tag;    // last used tag in TMtable
	struct Ref*         refArray;    // locked objects
	int                 refSize;     // size of refArray
	int                 refFree;     // list of free positions in refArray
	unsigned long       GCthreshold;
	unsigned long       nblocks;     // number of `bytes' currently allocated
	lua_Hook            callhook;
	lua_Hook            linehook;
	int                 allowhooks;
}
lua_State;


#include "llimits.c"
#include "lzio.c"
//typedef struct Lua_State lua_State;

Number luaO_power2(Number n)
{
	Number p;

	p = MINPOWER2;
	
	while(p <= n)
  		p <<= 1;

	return p;
}


#include "lmem.c"
#include "lobject.c"

void luaD_call (lua_State *L, StkId func, int nResults);

#include "lopcodes.c"
#include "lundump.c"

#define incr_top {if (L->top == L->stack_last) luaD_checkstack(L, 1); L->top++;}
void luaD_checkstack(lua_State *L, int n);

#include "ltm.c"
#include "ldo.c"
//#include "ltable.c"
//#include "lmem.c"
#include "lstate.c"*/


#include "src/lzio.c"
#include "src/lmem.c"
#include "src/lundump.c"
#include "src/lgc.c"
#include "src/lparser.c"
#include "src/llex.c"
#include "src/lcode.c"
#include "src/lstring.c"
#include "src/lvm.c"
#include "src/lapi.c"
#include "src/lfunc.c"
#include "src/ldebug.c"
#include "src/ltm.c"
#include "src/ldo.c"
#include "src/lobject.c"
#include "src/ltable.c"
#include "src/lstate.c"


#include <types.c>


#define LUA_REFREGISTRY	0

typedef struct Lua_State Lua_State;

typedef enum
{
	LUA_USERDATA = 0,
	LUA_NIL      = 1,
	LUA_NUMBER   = 2,
	LUA_STRING   = 3,
	LUA_TABLE    = 4,
	LUA_FUNCTION = 5
}
Lua_Type;

typedef struct
{
	Byte*   event;
	Number  current_line;
	Byte*   name;
	Byte*   name_what;
	Number  number_of_upvalues;
	Number  line_defined;
	Byte*   what;
	Byte*   source;
	Byte    short_src[];
}
Lua_Debug;


typedef void   (*Lua_Hook)(Lua_State* state, Lua_Debug* debug);
typedef Number (*Lua_Function)(Lua_State* state);


#define lua_pop(state, n) lua_settop(state, -(n)-1)
#define lua_register(state, name, f) (lua_pushcclosure (state, f, 0), lua_setglobal (state, name))
#define lua_getfield(state, table_index_in_stack, field_name) (lua_pushstring(state, field_name), lua_gettable(state, table_index_in_stack))
#define lua_setfield(state, table_index_in_stack, field_name) {lua_pushstring(state, field_name); lua_pushvalue(state, -2); lua_settable(state, table_index_in_stack);}


//#include <lua.c>

#include <string.c>


void to_wide_string(Byte* utf8_string, Number16* wide_string, Number32 number_of_wide_characters)
{
	Number32 character;

	while(number_of_wide_characters > 0)
	{
		character = read_next_UTF_8_character_from_string(&utf8_string);

		if(!character)
			break;

		*wide_string = character;
		++wide_string;
		--number_of_wide_characters;
	}

	if(number_of_wide_characters)
		*wide_string = '\0';
}

void to_utf8_string(Number16* wide_string, Byte* utf8_string)
{
	Number16 character;

	for(;;)
	{
		character = *wide_string;
		write_UTF8_character(character, &utf8_string);

		if(!character)
			break;

		++wide_string;
	}
}


#include "libs/std.c"
#include "libs/gui.c"
#include "libs/fs.c"


Boolean file_changed = 0;


Boolean execute(Byte* file_name)
{
	Lua_State* state;
	Message    message;

restart:
	state = lua_open(65536);
	if(!state)
	{
		printf("not enough memory\n");
		return;
	}

	register_lua_std(state);
	register_lua_GUI(state);
	register_lua_fs(state);

	if(lua_dofile(state, file_name))
		goto error;

	while(!require_exit && !file_changed)
	{
		while(PeekMessageA(&message, 0, 0, 0, REMOVE_MESSAGE_FROM_QUEUE))
		{
			TranslateMessage(&message);
			DispatchMessageA(&message);
		}

		SleepEx(1, 1);
	}

	/*
	while(!require_exit && GetMessageW(&message, 0, 0, 0))
	{
		TranslateMessage(&message);
		DispatchMessageW(&message);

		//luaC_collectgarbage(state);
	}*/

	lua_close(state);

	if(file_changed)
	{
		file_changed = 0;
		require_exit = 0;
		goto restart;
	}

	return 1;

error:
	lua_close(state);
	return 0;
}


void reload_lua_file(Byte* file_name)
{
	file_changed = 1;
}


void main(Number number_of_arguments, Byte** arguments)
//stdcall Number32 WinMain(Byte* instance, Byte* previouse_instance, Byte* arguments, Number32 show_options)
{
	Byte*      script_path;
	Lua_State* state;
	Message    message;

	if(number_of_arguments > 1)
		script_path = arguments[1];
	else
		script_path = "main.lua";

	//if(*arguments)
	//	script_path = arguments;
	//else
	//	script_path = "main.lua";


	//while(lua_dofile(state, script_path))
	//{
		//system("cls");
		//printf("%s\n", lua_tostring(state, -1));
		//lua_pop(state, lua_gettop(state));
		//wait_for_directory_changes();
		//system("cls");
	//}

	Directory_Changes changes;
	read_directory_changes(&changes, &reload_lua_file, script_path);

	execute(script_path);
	

	//lua_pop(state, lua_gettop(state));
	//lua_close(state);

	return exit_code;
}