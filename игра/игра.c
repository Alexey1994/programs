#include <lua.c>
#include <graphics/3D/object/wavefront.c>
#include <system/thread.c>


void from_cstring_to_wstring(Byte* cstring, Stack* wstring, Byte* postfix)
{
	Number16 character;

	for(;;)
	{
		character = *cstring;
		
		if(!character)
			break;

		add_bytes_in_stack(wstring, &character, sizeof(character));
		++cstring;
	}

	for(;;)
	{
		character = *postfix;

		if(!character)
			break;

		add_bytes_in_stack(wstring, &character, sizeof(character));
		++postfix;
	}

	character = 0;
	add_bytes_in_stack(wstring, &character, sizeof(character));
}


Number lua_draw_Wavefront_Object(Lua_State* state)
{
	Number number_of_arguments;
	Wavefront_Object* object3D;

	number_of_arguments = lua_gettop(state);
	if(number_of_arguments != 1)
	{
		lua_pop(state, number_of_arguments);
		return 0;
	}

	lua_getfield(state, 1, "data");
	object3D = lua_touserdata(state, 2);
	if(object3D)
		draw_Wavefront_Object(object3D);
	lua_pop(state, 2);

	return 0;
}

Number lua_destroy_object3D_tag;

Number lua_load_Object3D(Lua_State* state)
{
	Number number_of_arguments;
	Byte*  object_name;

	number_of_arguments = lua_gettop(state);
	if(number_of_arguments != 1)
	{
		lua_pop(state, number_of_arguments);
		return 0;
	}

	object_name = lua_tostring(state, 1);
	lua_pop(state, 1);

	lua_newtable(state);

	Wavefront_Object* object3D = lua_newuserdata(state, sizeof(Wavefront_Object));
	lua_settag(state, lua_destroy_object3D_tag);
	lua_setfield(state, 1, "data");
	lua_pop(state, 1);
	
	if(object3D)
	{
		Stack long_object_name;
		initialize_stack(&long_object_name, 16);
		from_cstring_to_wstring(object_name, &long_object_name, "/.obj");
		read_Wavefront_Object(long_object_name.data, object3D);
		deinitialize_stack(&long_object_name);
	}
	else
		printf("out of memory\n");

	lua_pushcclosure(state, &lua_draw_Wavefront_Object, 0);
	lua_setfield(state, 1, "draw");

	lua_pop(state, number_of_arguments);

	return 1;
}

Number lua_destroy_object3D(Lua_State* state)
{
	Number number_of_arguments;
	Wavefront_Object* object3D;

	number_of_arguments = lua_gettop(state);
	if(number_of_arguments != 1)
	{
		lua_pop(state, number_of_arguments);
		return 0;
	}

	object3D = lua_touserdata(state, 1);
	if(object3D)
		deinitialize_Wavefront_Object(object3D);
	lua_pop(state, 1);

	return 0;
}


Number lua_rotate3D(Lua_State* state)
{
	Number number_of_arguments;
	Rational_Number64 angle;
	Rational_Number64 x;
	Rational_Number64 y;
	Rational_Number64 z;

	number_of_arguments = lua_gettop(state);
	if(number_of_arguments != 4)
	{
		lua_pop(state, number_of_arguments);
		return 0;
	}

	angle = lua_tonumber(state, 1);
	x = lua_tonumber(state, 2);
	y = lua_tonumber(state, 3);
	z = lua_tonumber(state, 4);
	glRotatef(angle, x, y, z);
	lua_pop(state, 4);

	return 0;
}


Number lua_translate3D(Lua_State* state)
{
	Number number_of_arguments;
	Rational_Number64 x;
	Rational_Number64 y;
	Rational_Number64 z;

	number_of_arguments = lua_gettop(state);
	if(number_of_arguments != 3)
	{
		lua_pop(state, number_of_arguments);
		return 0;
	}

	x = lua_tonumber(state, 1);
	y = lua_tonumber(state, 2);
	z = lua_tonumber(state, 3);
	glTranslatef(x, y, z);
	lua_pop(state, 3);

	return 0;
}


Number lua_scale3D(Lua_State* state)
{
	Number number_of_arguments;
	Rational_Number64 x;
	Rational_Number64 y;
	Rational_Number64 z;

	number_of_arguments = lua_gettop(state);
	if(number_of_arguments != 3)
	{
		lua_pop(state, number_of_arguments);
		return 0;
	}

	x = lua_tonumber(state, 1);
	y = lua_tonumber(state, 2);
	z = lua_tonumber(state, 3);
	glScalef(x, y, z);
	lua_pop(state, 3);

	return 0;
}


Number lua_push_transform(Lua_State* state)
{
	Number number_of_arguments;

	number_of_arguments = lua_gettop(state);
	if(number_of_arguments != 0)
	{
		lua_pop(state, number_of_arguments);
		return 0;
	}

	glPushMatrix();

	return 0;
}


Number lua_pop_transform(Lua_State* state)
{
	Number number_of_arguments;

	number_of_arguments = lua_gettop(state);
	if(number_of_arguments != 0)
	{
		lua_pop(state, number_of_arguments);
		return 0;
	}

	glPopMatrix();

	return 0;
}


void add_messages_in_lua(Lua_State* state, Graphics* graphics)
{
	Message* message;
	Number   i;

	lua_newtable(state);
	lua_setglobal(state, "events");
	lua_getglobal(state, "events");

	for(i = 0; i < graphics->number_of_messages && i < MAX_NUMBER_OF_MESSAGES; ++i)
	{
		message = graphics->messages + i;

		lua_newtable(state);
		lua_pushstring(state, "type");
		lua_pushnumber(state, message->type);
		lua_settable(state, -3);
		lua_pushstring(state, "x");
		lua_pushnumber(state, message->point.x);
		lua_settable(state, -3);
		lua_pushstring(state, "y");
		lua_pushnumber(state, message->point.y);
		lua_settable(state, -3);
		lua_pushstring(state, "parameter1");
		lua_pushnumber(state, message->parameter1);
		lua_settable(state, -3);
		lua_pushstring(state, "parameter2");
		lua_pushnumber(state, message->parameter2);
		lua_settable(state, -3);

		lua_rawseti(state, -2, i + 1);
	}
}


void register_game(Lua_State* state)
{
	lua_register(state, "print", &lua_print);
	lua_register(state, "require", &lua_require);
	lua_register(state, "length", &lua_length);

	lua_destroy_object3D_tag = lua_newtag(state);
	lua_pushcclosure(state, &lua_destroy_object3D, 0);
	lua_settagmethod(state, lua_destroy_object3D_tag, "gc");

	lua_register(state, "load_Object3D", &lua_load_Object3D);
	lua_register(state, "rotate3D", &lua_rotate3D);
	lua_register(state, "translate3D", &lua_translate3D);
	lua_register(state, "scale3D", &lua_scale3D);
	lua_register(state, "push_transform", &lua_push_transform);
	lua_register(state, "pop_transform", &lua_pop_transform);
}


void main()
{
	Graphics graphics;

	initialize_graphics(&graphics, 640, 480);

	glEnable(GL_LIGHTING);
	Rational_Number32 diffuse_color[]  = {1, 1, 1, 1};
	Rational_Number32 specular_color[] = {1, 1, 1, 1};
	Rational_Number32 ambient_color[]  = {0.9, 0.9, 0.9, 0};
	Rational_Number32 light_position[] = {0.0, 0.0, -10.0, 1};

	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_color);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular_color);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_color);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glEnable(GL_LIGHT0);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_NORMALIZE);
	glEnable(GL_TEXTURE_2D);


	Lua_State* state;

	state = lua_open();
	if(!state)
	{
		printf("not enough memory\n");
		return;
	}

	register_game(state);

	while(lua_dofile(state, "scripts/init.lua"))
	{
		system("cls");
		printf("%s\n", lua_tostring(state, -1));
		lua_pop(state, lua_gettop(state));
		wait_for_directory_changes();
		system("cls");
	}

	for(;;)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		update_messages(&graphics);
		add_messages_in_lua(state, &graphics);
		//print_messages(&graphics);

		//Message* mouse_move_message = find_message(&graphics, MOUSE_MOVE_MESSAGE);
		//if(mouse_move_message)
		//	printf("%d, %d\n", mouse_move_message->point.x, mouse_move_message->point.y);

		if(find_message(&graphics, QUIT_MESSAGE))
			break;

		if(lua_dofile(state, "scripts/game.lua"))
		{
			system("cls");
			printf("%s\n", lua_tostring(state, -1));
			wait_for_directory_changes();
			system("cls");
		}
		lua_pop(state, lua_gettop(state));

		draw_graphics(&graphics);
		sleep_thread(10);
	}

	lua_pop(state, lua_gettop(state));
	lua_close(state);
}