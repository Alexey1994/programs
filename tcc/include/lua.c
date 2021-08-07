#ifndef LUA_INCLUDED
#define LUA_INCLUDED


//https://www.lua.org/manual/4.0/manual.html


#include <types.c>


//#define LUAI_MAXSTACK		15000 //1000000 for 64 bits
//#define LUA_REGISTRYINDEX	(-LUAI_MAXSTACK - 1000)

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

import Lua_State*        lua_open           ();
import void              lua_close          (Lua_State* state);
import Number            lua_dofile         (Lua_State* state, Byte* file_name);
import Number            lua_dostring       (Lua_State* state, Byte* file_name);
import Number            lua_dobuffer       (Lua_State* state, Byte* buffer, Number size_of_buffer, Byte* debug_name);
import void              lua_error          (Lua_State* state, Byte* message);

import Number            lua_equal          (Lua_State* state, Number index_in_stack1, Number index_in_stack2);
import Number            lua_lessthan       (Lua_State* state, Number index_in_stack1, Number index_in_stack2);

import void              lua_getglobal      (Lua_State* state, Byte* varname);
import void              lua_getglobals     (Lua_State* state);
import Number            lua_setglobal      (Lua_State* state, Byte* field);
import void              lua_setglobals     (Lua_State* state);

import void              lua_newtable       (Lua_State* state);
import Number            lua_newuserdata    (Lua_State* state, Number size);
import void              lua_gettable       (Lua_State* state, Number index_in_stack);
import void              lua_settable       (Lua_State* state, Number index_in_stack);
import Number            lua_getn           (Lua_State* state, Number index_in_stack);
import void              lua_rawget         (Lua_State* state, Number index_in_stack);
import void              lua_rawgeti        (Lua_State* state, Number index_in_stack, Number key_index);
import void              lua_rawset         (Lua_State* state, Number index_in_stack);
import void              lua_rawseti        (Lua_State* state, Number index_in_stack, Number key_index);
import Number            lua_next           (Lua_State* state, Number index_in_stack);

import Number            lua_newtag         (Lua_State* state);
import void              lua_settag         (Lua_State* state, Number tag);
import void              lua_settagmethod   (Lua_State* state, Number tag, Byte* event_name);
import void              lua_gettagmethod   (Lua_State* state, Number tag, Byte* event_name);
import Number            lua_copytagmethods (Lua_State* state, Number tag_to, Number tag_from);

import Number            lua_ref            (Lua_State* state, Boolean lock);
import Number            lua_getref         (Lua_State* state, Number ref);
import void              lua_unref          (Lua_State* state, Number ref);

import Number            lua_stackspace     (Lua_State* state);
import void              lua_settop         (Lua_State* state, Number top_index);
import Number            lua_gettop         (Lua_State* state);
import void              lua_remove         (Lua_State* state, Number index_in_stack);
import void              lua_insert         (Lua_State* state, Number index_in_stack);
import void              lua_concat         (Lua_State* state, Number number_of_values);

import void              lua_pushnil        (Lua_State* state);
import void              lua_pushnumber     (Lua_State* state, Rational_Number64 value);
import void              lua_pushstring     (Lua_State* state, Byte* string);
import void              lua_pushlstring    (Lua_State* state, Byte* string, Number string_length);
import void              lua_pushvalue      (Lua_State* state, Number index_in_stack);
import void              lua_pushcclosure   (Lua_State* state, Lua_Function function, Number number_of_values);
import void              lua_pushusertag    (Lua_State* state, void* u, Number tag);

import Lua_Type          lua_type           (Lua_State* state, Number index_in_stack);
import Byte*             lua_typename       (Lua_State* state, Number index_in_stack);
import Number            lua_tag            (Lua_State* state, Number index_in_stack);
//import Boolean           lua_isnil          (Lua_State* state, Number index_in_stack);
import Boolean           lua_isnumber       (Lua_State* state, Number index_in_stack);
import Boolean           lua_isstring       (Lua_State* state, Number index_in_stack);
//import Boolean           lua_istable        (Lua_State* state, Number index_in_stack);
//import Boolean           lua_isfunction     (Lua_State* state, Number index_in_stack);
import Boolean           lua_iscfunction    (Lua_State* state, Number index_in_stack);
//import Boolean           lua_isuserdata     (Lua_State* state, Number index_in_stack);

import Rational_Number64 lua_tonumber       (Lua_State* state, Number index_in_stack);
import Byte*             lua_topointer      (Lua_State* state, Number index_in_stack);
import Byte*             lua_tostring       (Lua_State* state, Number index_in_stack);
import Number            lua_strlen         (Lua_State* state, Number index_in_stack);
import Byte*             lua_touserdata     (Lua_State* state, Number index_in_stack);
import Lua_Function      lua_tocfunction    (Lua_State* state, Number index_in_stack);

import void              lua_call           (Lua_State* state, Number number_of_arguments, Number number_of_results);
import void              lua_rawcall        (Lua_State* state, Number number_of_arguments, Number number_of_results);

import Byte*             lua_setlocal       (Lua_State* state, Lua_Debug* debug, Number index_of_local_variable);
import Byte*             lua_getlocal       (Lua_State* state, Lua_Debug* debug, Number index_of_local_variable);

import Number            lua_getgccount     (Lua_State* state);
import Number            lua_getgcthreshold (Lua_State* state);
import void              lua_setgcthreshold (Lua_State* state, Number new_threshold);

import Number            lua_getinfo        (Lua_State* state, Byte* what, Lua_Debug* debug_name);
import Number            lua_getstack       (Lua_State* state, Number level, Lua_Debug* debug);
import Lua_Hook          lua_setcallhook    (Lua_State* state, Lua_Hook hook);
import Lua_Hook          lua_setlinehook    (Lua_State* state, Lua_Hook hook);


#define lua_pop(state, n) lua_settop(state, -(n)-1)
#define lua_register(state, name, f) (lua_pushcclosure (state, f, 0), lua_setglobal (state, name))
#define lua_getfield(state, table_index_in_stack, field_name) (lua_pushstring(state, field_name), lua_gettable(state, table_index_in_stack))
#define lua_setfield(state, table_index_in_stack, field_name) {lua_pushstring(state, field_name); lua_pushvalue(state, -2); lua_settable(state, table_index_in_stack);}


Number lua_require(Lua_State* state)
{
	Number number_of_arguments;
	Byte*  file_name;

	number_of_arguments = lua_gettop(state);
	if(number_of_arguments != 1)
	{
		lua_pop(lua_gettop(state), number_of_arguments);
		return 0;
	}

	file_name = lua_tostring(state, -1);
	lua_pop(state, 1);
	lua_dofile(state, file_name);

	return lua_gettop(state);
}


Number lua_length(Lua_State* state)
{
	Number number_of_arguments;
	Number length;

	number_of_arguments = lua_gettop(state);
	if(number_of_arguments != 1)
	{
		lua_pop(lua_gettop(state), number_of_arguments);
		return 0;
	}

	length = lua_getn(state, -1);
	lua_pop(state, 1);
	lua_pushnumber(state, length);

	return 1;
}


static void print_spaces(Number number_of_spaces)
{
	while(number_of_spaces)
	{
		printf("    ");
		--number_of_spaces;
	}
}

static void print_lua_value(Lua_State* state, Number index_in_stack, Number level, Boolean need_spaces)
{
	Byte*             string;
	Rational_Number64 number;

	if(need_spaces)
		print_spaces(level);

	switch(lua_type(state, index_in_stack))
	{
		case LUA_NIL:
			printf("nil");
			break;

		case LUA_NUMBER:
			number = lua_tonumber(state, index_in_stack);
			printf("%g", number);
			break;

		case LUA_STRING:
			string = lua_tostring(state, index_in_stack);
			printf("%s", string);
			break;

		case LUA_TABLE:
			printf("{\n");

			lua_pushnil(state);
			while(lua_next(state, index_in_stack) != 0)
			{
				print_lua_value(state, lua_gettop(state) - 1, level + 1, 1);
				printf(" = ");
				print_lua_value(state, lua_gettop(state), level + 1, 0);
				printf("\n");

				lua_remove(state, -1);
			}

			print_spaces(level);
			printf("}");
			break;

		case LUA_FUNCTION:
			printf("function\n");
			break;

		case LUA_USERDATA:
			printf("userdata\n");
			break;

		default:
			printf("undefined type %d\n", lua_type(state, index_in_stack));
	}
}


Number lua_print(Lua_State* state)
{
	Number number_of_arguments;

	number_of_arguments = lua_gettop(state);

	while(number_of_arguments)
	{
		print_lua_value(state, 1, 0, 1);

		lua_remove(state, 1);
		--number_of_arguments;
	}

	printf("\n");

	return 0;
}


#endif//LUA_INCLUDED

/*
Number lua_gc(Lua_State* state)
{
	printf("destroy %d\n", lua_gettop(state));
}


void main()
{
	Lua_State* state;

	state = lua_open();
	if(!state)
	{
		printf("not enough memory\n");
		return;
	}

	lua_register(state, "print", &lua_print);
	lua_register(state, "require", &lua_require);


	lua_dostring(state, "print(1, 2, 3)");
	lua_dostring(state, "print(9.8)");
	lua_dostring(state, "print({abc = 'a', b = 9, c = {a = 8, b = 9, c = {a = 1, b = 2, c = 3}, d = 5}}, {x = 1, y = 2})");
	//lua_dofile(state, "main.lua");

	lua_dostring(state, "t = {a = 112}");
	lua_getglobal(state, "t");
	lua_pushstring(state, "a");
	lua_gettable(state, -2);
	printf("%g\n", lua_tonumber(state, -1));


	lua_newuserdata(state, 12);
	lua_setglobal(state, "t");

	Number tag = lua_newtag(state);
	lua_pushcclosure(state, &lua_gc, 0);
	lua_settagmethod(state, tag, "gc");

	lua_getglobal(state, "t");
	lua_settag(state, tag);
	lua_pop(state, lua_gettop(state));

	lua_pushnil(state);
	lua_setglobal(state, "t");
	lua_pop(state, lua_gettop(state));


	lua_close(state);
}*/