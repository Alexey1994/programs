#include <lua.c>
#include <data-structures/stack.c>
#include <system/Windows/user32.c>
#include <system/Windows/gdi32.c>
#include <system/thread.c>
#include <system/file.c>
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


typedef struct
{
	Lua_State* state;
	Byte*      window;
	Byte*      paint_device;
	Number     background;
	Number     on_paint;
	Number     on_left_mouse_button_up;
}
Lua_Window;

Number lua_paint_text(Lua_State* state)
{
	Number number_of_arguments;

	number_of_arguments = lua_gettop(state);
	if(number_of_arguments != 7)
	{
		lua_pop(state, number_of_arguments);
		return 0;
	}

	Byte**   paint_device;
	Number16 text[256];
	Byte**   font;
	Number   x;
	Number   y;
	Number   width;
	Number   height;

	lua_getfield(state, 1, "paint_device");
	paint_device = lua_touserdata(state, -1);

	to_wide_string(lua_tostring(state, 2), text, 256);
	font = lua_touserdata(state, 3);

	x = lua_tonumber(state, 4);
	y = lua_tonumber(state, 5);
	width = lua_tonumber(state, 6);
	height = lua_tonumber(state, 7);

	if(paint_device)
	{
		SelectObject(*paint_device, *font);
		Windows_Rectangle text_region = {x, y, x + width, y + height};
		DrawTextW(*paint_device, text, -1, &text_region, DT_LEFT | DT_TOP | DT_END_ELLIPSIS);
	}

	lua_pop(state, lua_gettop(state));

	return 0;
}

Number lua_paint_rectangle(Lua_State* state)
{
	Number number_of_arguments;

	number_of_arguments = lua_gettop(state);
	if(number_of_arguments != 6)
	{
		lua_pop(state, number_of_arguments);
		return 0;
	}

	Byte**  paint_device;
	Number  x;
	Number  y;
	Number  width;
	Number  height;
	Number* color;

	lua_getfield(state, 1, "paint_device");
	paint_device = lua_touserdata(state, -1);

	x = lua_tonumber(state, 2);
	y = lua_tonumber(state, 3);
	width = lua_tonumber(state, 4);
	height = lua_tonumber(state, 5);
	color = lua_touserdata(state, 6);

	if(paint_device)
	{
		Windows_Rectangle rectangle = {x, y, x + width, y + height};
		FillRect(*paint_device, &rectangle, *color);
	}

	lua_pop(state, lua_gettop(state));

	return 0;
}

stdcall Number32 update_window_state(Byte* window, Number32 message, Number32* parameter1, Integer_Number32* parameter2)
{
	Lua_Window* target_window;

	switch(message)
	{
		case WM_LBUTTONUP:
			target_window = GetWindowLongW(window, GWL_USERDATA);
			
			if(target_window->on_left_mouse_button_up)
			{
				lua_getref(target_window->state, target_window->on_left_mouse_button_up);
				lua_pushnumber(target_window->state, parameter1);
				lua_call(target_window->state, 1, 0);
			}
			break;

		case KEY_DOWN_MESSAGE:
			//print("key down\n")
			break;

		case WM_LBUTTONDOWN:
			//print("lbutton down\n")
			break;

		case PAINT_MESSAGE:
			target_window = GetWindowLongW(window, GWL_USERDATA);
			
			if(target_window->on_paint)
			{
				Byte** paint_device;
				Windows_Paint paint = {
					.erase_background = 1
				};

				lua_getref(target_window->state, target_window->on_paint);

				lua_newtable(target_window->state);
				{
					paint_device = lua_newuserdata(target_window->state, sizeof(Byte*));
					*paint_device = BeginPaint(window, &paint);

					SetBkMode(*paint_device, TRANSPARENT);

					lua_setfield(target_window->state, 2, "paint_device");
					lua_pop(target_window->state, 1);

					lua_pushcclosure(target_window->state, &lua_paint_text, 0);
					lua_setfield(target_window->state, 2, "text");
					lua_pop(target_window->state, 1);

					lua_pushcclosure(target_window->state, &lua_paint_rectangle, 0);
					lua_setfield(target_window->state, 2, "rectangle");
					lua_pop(target_window->state, 1);
				}

				lua_call(target_window->state, 1, 0);
				EndPaint(window, &paint);
			}
			break;

		case MOVE_MESSAGE:
			//print("move\n")
			break;

		case SETFOCUS_MESSAGE:
			//print("set focus\n")
			break;

		case KILLFOCUS_MESSAGE:
			//print("kill focus\n")
			break;

		case CREATE_MESSAGE:

			break;

		case SETFONT_MESSAGE:
			break;
	}

	return DefWindowProcW(window, message, parameter1, parameter2);
}

Number lua_rgb(Lua_State* state)
{
	Number number_of_arguments;

	number_of_arguments = lua_gettop(state);
	if(number_of_arguments != 3)
	{
		lua_pop(state, number_of_arguments);
		return 0;
	}

	Number r;
	Number g;
	Number b;

	r = lua_tonumber(state, 1);
	g = lua_tonumber(state, 2);
	b = lua_tonumber(state, 3);
	lua_pop(state, 3);

	Number* color = lua_newuserdata(state, sizeof(Number));
	*color = CreateSolidBrush(r | (g << 8) | (b << 16));

	return 1;
}

Number lua_image(Lua_State* state)
{
	Number number_of_arguments;

	number_of_arguments = lua_gettop(state);
	if(number_of_arguments != 1)
	{
		lua_pop(state, number_of_arguments);
		return 0;
	}

	Number16 name[256];

	to_wide_string(lua_tostring(state, -1), name, 256);
	lua_pop(state, 1);

	Number* image = lua_newuserdata(state, sizeof(Byte*));
	*image = CreatePatternBrush(LoadImageW(0, name, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE));

	return 1;
}

Number lua_font(Lua_State* state)
{
	Number number_of_arguments;

	number_of_arguments = lua_gettop(state);
	if(number_of_arguments != 2)
	{
		lua_pop(state, number_of_arguments);
		return 0;
	}

	Number   size;
	Number16 font_name[256];

	size = lua_tonumber(state, -1);
	lua_pop(state, 1);

	to_wide_string(lua_tostring(state, -1), font_name, 256);
	lua_pop(state, 1);

	Byte** font = lua_newuserdata(state, sizeof(Byte*));
	*font = CreateFontW(size, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, font_name);

	return 1;
}

Number lua_set_font(Lua_State* state)
{
	Number number_of_arguments;

	number_of_arguments = lua_gettop(state);
	if(number_of_arguments != 2)
	{
		lua_pop(state, number_of_arguments);
		return 0;
	}

	Number   size;
	Number16 font_name[256];

	size = lua_tonumber(state, -1);
	lua_pop(state, 1);

	to_wide_string(lua_tostring(state, -1), font_name, 256);
	lua_pop(state, 1);

	lua_pushnumber(state, CreateFontW(size, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, font_name));

	return 1;
}

Number lua_create_window_class(Lua_State* state)
{
	Number number_of_arguments;

	number_of_arguments = lua_gettop(state);
	if(number_of_arguments != 1)
	{
		lua_pop(state, number_of_arguments);
		return 0;
	}

	Number16 name[256];
	Byte*    short_name;
	Number*  background;

	lua_pushstring(state, "name");
	lua_gettable(state, 1);
	short_name = lua_tostring(state, -1);
	to_wide_string(short_name, name, 256);
	lua_pop(state, 1);

	lua_pushstring(state, "background");
	lua_gettable(state, 1);
	background = lua_touserdata(state, -1);
	lua_pop(state, 1);

	lua_pop(state, 1);

	Window_Class window_class = {
		.structure_size   = sizeof(Window_Class),
		.window_procedure = &update_window_state,
		.cursor           = LoadCursorA(0, DEFAULT_ARROW),
		.background       = *background,
		.class_name       = name
	};

	RegisterClassExW(&window_class);

	/*lua_newtable(state);
	{
		lua_pushstring(state, short_name);
		lua_setfield(state, 1, "name");
		lua_pop(state, 1);
	}*/
	lua_pushstring(state, short_name);

	return 1;
}

Number lua_show_window(Lua_State* state)
{
	Number number_of_arguments;
	Lua_Window* window;

	number_of_arguments = lua_gettop(state);
	if(number_of_arguments != 1)
	{
		lua_pop(state, number_of_arguments);
		return 0;
	}

	lua_getfield(state, 1, "data");
	window = lua_touserdata(state, 2);
	if(window)
		ShowWindow(window->window, SHOW_DEFAULT_WINDOW);
	lua_pop(state, 2);

	return 0;
}

Number lua_hide_window(Lua_State* state)
{
	Number number_of_arguments;
	Lua_Window* window;

	number_of_arguments = lua_gettop(state);
	if(number_of_arguments != 1)
	{
		lua_pop(state, number_of_arguments);
		return 0;
	}

	lua_getfield(state, 1, "data");
	window = lua_touserdata(state, 2);
	if(window)
		ShowWindow(window->window, HIDE_WINDOW);
	lua_pop(state, 2);

	return 0;
}

Number lua_destroy_window_tag;

Number lua_destroy_window(Lua_State* state)
{
	Number number_of_arguments;

	number_of_arguments = lua_gettop(state);
	if(number_of_arguments != 1)
	{
		lua_pop(state, number_of_arguments);
		return 0;
	}

	Lua_Window* window;
	window = lua_touserdata(state, 1);
	if(window)
		DestroyWindow(window->window);
	lua_pop(state, 1);

	return 0;
}

Number lua_create_window(Lua_State* state)
{
	Number number_of_arguments;

	number_of_arguments = lua_gettop(state);
	if(number_of_arguments != 1)
	{
		lua_pop(state, number_of_arguments);
		return 0;
	}

	Lua_Window* parent_lua_window;

	Number16 class[256];
	Number16 name[256];
	Number   x;
	Number   y;
	Number   width;
	Number   height;

	Number on_paint;
	Number on_left_mouse_button_up;

	lua_pushstring(state, "parent");
	lua_gettable(state, 1);
	if(lua_topointer(state, -1))
	{
		lua_pushstring(state, "data");
		lua_gettable(state, 2);
		parent_lua_window = lua_touserdata(state, -1);
		lua_pop(state, 2);
	}
	else
	{
		parent_lua_window = 0;
		lua_pop(state, 1);
	}

	lua_pushstring(state, "class");
	lua_gettable(state, 1);
	to_wide_string(lua_tostring(state, -1), class, 256);
	lua_pop(state, 1);

	lua_pushstring(state, "name");
	lua_gettable(state, 1);
	to_wide_string(lua_tostring(state, -1), name, 256);
	lua_pop(state, 1);

	lua_pushstring(state, "x");
	lua_gettable(state, 1);
	x = lua_tonumber(state, -1);
	lua_pop(state, 1);

	lua_pushstring(state, "y");
	lua_gettable(state, 1);
	y = lua_tonumber(state, -1);
	lua_pop(state, 1);

	lua_pushstring(state, "width");
	lua_gettable(state, 1);
	width = lua_tonumber(state, -1);
	lua_pop(state, 1);

	lua_pushstring(state, "height");
	lua_gettable(state, 1);
	height = lua_tonumber(state, -1);
	lua_pop(state, 1);

	lua_pushstring(state, "on_paint");
	lua_gettable(state, 1);
	if(lua_topointer(state, -1))
		on_paint = lua_ref(state, 0);
	else
	{
		on_paint = 0;
		lua_pop(state, 1);
	}

	lua_pushstring(state, "on_left_mouse_button_up");
	lua_gettable(state, 1);
	if(lua_topointer(state, -1))
		on_left_mouse_button_up = lua_ref(state, 0);
	else
	{
		on_left_mouse_button_up = 0;
		lua_pop(state, 1);
	}

	lua_pop(state, 1);

	Byte*    parent_window;
	Byte*    instance;
	Number32 window_style;
	
	if(parent_lua_window)
	{
		window_style = WS_CHILD | WS_VISIBLE;
		parent_window = parent_lua_window->window;
		instance = GetWindowLongW(parent_window, GWL_HINSTANCE);
	}
	else
	{
		window_style = WS_OVERLAPPED | WS_SYSMENU;
		parent_window = 0;
		instance = 0;
	}

	Byte* window = CreateWindowExW(
		0,//WS_EX_NOACTIVATE,
		class,
		name,
		window_style,
		x,
		y,
		width,
		height,
		parent_window,
		0,
		instance,
		0
	);

	lua_newtable(state);
	{
		Lua_Window* lua_window = lua_newuserdata(state, sizeof(Lua_Window));
		lua_window->state      = state;
		lua_window->window     = window;
		lua_window->on_paint                = on_paint;
		lua_window->on_left_mouse_button_up = on_left_mouse_button_up;

		lua_settag(state, lua_destroy_window_tag);
		lua_setfield(state, 1, "data");
		lua_pop(state, 1);

		lua_pushcclosure(state, &lua_show_window, 0);
		lua_setfield(state, 1, "show");
		lua_pop(state, 1);

		lua_pushcclosure(state, &lua_hide_window, 0);
		lua_setfield(state, 1, "hide");
		lua_pop(state, 1);

		SetWindowLongW(window, GWL_USERDATA, lua_window);
	}

	return 1;
}


void register_GUI(Lua_State* state)
{
	lua_register(state, "print", &lua_print);
	lua_register(state, "require", &lua_require);
	lua_register(state, "length", &lua_length);

	lua_destroy_window_tag = lua_newtag(state);
	lua_pushcclosure(state, &lua_destroy_window, 0);
	lua_settagmethod(state, lua_destroy_window_tag, "gc");

	lua_register(state, "font", &lua_font);
	lua_register(state, "rgb", &lua_rgb);
	lua_register(state, "image", &lua_image);
	lua_register(state, "window_class", &lua_create_window_class);
	lua_register(state, "window", &lua_create_window);
}


void main(Number number_of_arguments, Byte** arguments)
{
	Byte*      script_path;
	Lua_State* state;

	if(number_of_arguments > 1)
		script_path = arguments[1];
	else
		script_path = "main.lua";

	state = lua_open();
	if(!state)
	{
		printf("not enough memory\n");
		return;
	}

	register_GUI(state);

	while(lua_dofile(state, script_path))
	{
		//system("cls");
		printf("%s\n", lua_tostring(state, -1));
		lua_pop(state, lua_gettop(state));
		wait_for_directory_changes();
		//system("cls");
	}

	for(;;)
	{
		Message message;

		while(GetMessageW(&message, 0, 0, 0))
		{
			TranslateMessage(&message);
			DispatchMessageW(&message);
		}

		sleep_thread(10);
	}

	lua_pop(state, lua_gettop(state));
	lua_close(state);
}