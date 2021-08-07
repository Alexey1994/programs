Number lua_require(Lua_State* state)
{
	Number number_of_arguments;

	number_of_arguments = lua_gettop(state);
	if(number_of_arguments != 1)
	{
		lua_pop(lua_gettop(state), number_of_arguments);
		return 0;
	}

	Byte* file_name;

	file_name = lua_tostring(state, -1);
	lua_pop(state, 1);
	lua_dofile(state, file_name);

	return lua_gettop(state);
}


Number lua_length(Lua_State* state)
{
	Number number_of_arguments;

	number_of_arguments = lua_gettop(state);
	if(number_of_arguments != 1)
	{
		lua_pop(lua_gettop(state), number_of_arguments);
		return 0;
	}

	Number length;

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


#include <system/Windows/shell32.c>

Number lua_execute(Lua_State* state)
{
	Number number_of_arguments;

	number_of_arguments = lua_gettop(state);
	if(number_of_arguments != 1)
	{
		lua_pop(lua_gettop(state), number_of_arguments);
		return 0;
	}

	Number16 command[256];

	to_wide_string(lua_tostring(state, 1), command, 256);
	lua_pop(state, 1);

	Sturtup_Info        startup_info = {.size = sizeof(Sturtup_Info)};
	Process_Information process_info;
	lua_pushnumber(state, CreateProcessW(L"C:\\Windows\\system32\\cmd.exe", command, 0, 0, 0, 0, 0, 0, &startup_info, &process_info));

	//lua_pushnumber(state, ShellExecuteA(0, 0, "C:\\Windows\\directx.log", 0, 0, 0));//ShellExecuteW(0, 0, command, 0, 0, 0));

	return 1;
}


Boolean require_exit = 0;
Number  exit_code;

Number lua_exit(Lua_State* state)
{
	Number number_of_arguments;

	number_of_arguments = lua_gettop(state);
	if(number_of_arguments != 1)
	{
		lua_pop(lua_gettop(state), number_of_arguments);
		return 0;
	}

	exit_code = lua_tonumber(state, -1);
	lua_pop(state, 1);
	require_exit = 1;

	return 0;
}


Number lua_is_table(Lua_State* state)
{
	Number number_of_arguments;

	number_of_arguments = lua_gettop(state);
	if(number_of_arguments != 1)
	{
		lua_pop(lua_gettop(state), number_of_arguments);
		return 0;
	}

	Boolean is_table;

	is_table = lua_type(state, -1) == LUA_TABLE;
	lua_pop(state, 1);

	if(is_table)
		lua_pushnumber(state, 1);
	else
		lua_pushnil(state);

	return 1;
}


void register_lua_std(Lua_State* state)
{
	lua_register(state, "require", &lua_require);
	lua_register(state, "length", &lua_length);
	lua_register(state, "print", &lua_print);

	lua_register(state, "execute", &lua_execute);
	lua_register(state, "exit", &lua_exit);

	lua_register(state, "is_table", &lua_is_table);
}