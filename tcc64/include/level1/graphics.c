#ifndef SYSTEM_GRAPHICS_INCLUDED
#define SYSTEM_GRAPHICS_INCLUDED


#include <types.c>


#define MAX_NUMBER_OF_MESSAGES 16


typedef enum
{
	UNKNOWN_GRAPHICS_MESSAGE,
	CREATE_GRAPHICS,
	DESTROY_GRAPHICS

}
Graphics_Message_Type;


typedef struct
{
	Graphics_Message_Type type;
}
Graphics_Message;


typedef struct
{
	Bit32            width;
	Bit32            height;
	Bit32*           data;
	Graphics_Message last_message;
	Byte*            window;
	Byte*            window_device_context;
	Byte*            opengl_context;
}
Graphics;


static void    initialize_graphics       (Graphics* graphics, Number32 x, Number32 y, Number32 width, Number32 height);
static void    begin_draw_graphics       (Graphics* graphics);
static void    end_draw_graphics         (Graphics* graphics);
static Boolean get_next_graphics_message (Graphics* graphics);
static void    deinitialize_graphics     (Graphics* graphics);


#ifdef __WIN32__


#include <Windows/gdi32.c>
#include <Windows/user32.c>
#include <Windows/opengl32.c>
#include <Windows/glu32.c>


static stdcall Number32 update_window_state(Byte* window, Windows_Message_Type type, Number32* parameters_1, Integer_Number32* parameters_2)
{
	switch(type)
	{
		case CLOSE_MESSAGE:
			PostMessageA(window, DESTROY_MESSAGE, 0, 0);
			return 0;

		case DESTROY_MESSAGE:
			return 0;
	}

	return DefWindowProcA(window, type, parameters_1, parameters_2);
}


static void initialize_graphics(Graphics* graphics, Number32 x, Number32 y, Number32 width, Number32 height)
{
	Number status;

	graphics->width              = width;
	graphics->height             = height;
	graphics->last_message.type  = UNKNOWN_GRAPHICS_MESSAGE;

	Window_Class window_class = {
		.structure_size   = sizeof(Window_Class),
		.style            = OWN_WINDOW_CONTEXT,
		.window_procedure = &update_window_state,
		.class            = 0,
		.window           = 0,
		.instance         = 0,
		.icon             = LoadIconA(0, DEFAULT_ICON),
		.cursor           = LoadCursorA(0, DEFAULT_ARROW),
		.background       = 0,
		.menu_name        = 0,
		.class_name       = "Main window class",
		.small_icon       = LoadIconA(0, DEFAULT_ICON)
	};

	status = RegisterClassExA(&window_class);

	if(!status)
		goto error;

	graphics->window = CreateWindowExA(
		0,
		"Main window class",
		"Main",
		WS_OVERLAPPED | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SIZEBOX,
		x,
		y,
		width,
		height,
		0,
		0,
		window_class.instance,
		0
	);

	if(!graphics->window)
	{
		//log_error("CreateWindowExA")
		goto error;
	}

	//SetWindowLongA(graphics->window, USER_DATA, graphics);
	ShowWindow(graphics->window, SHOW_DEFAULT_WINDOW);

	graphics->window_device_context = GetDC(graphics->window);

	Windows_Pixel_Format_Descriptor pixel_format_descriptor = {
		.size            = sizeof(Windows_Pixel_Format_Descriptor),
		.nVersion        = 1,
		.dwFlags         = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		.iPixelType      = PFD_TYPE_RGBA,
		.cColorBits      = 32,
		.cRedBits        = 0,
		.cRedShift       = 0,
		.cGreenBits      = 0,
		.cGreenShift     = 0,
		.cBlueBits       = 0,
		.cBlueShift      = 0,
		.cAlphaBits      = 0,
		.cAlphaShift     = 0,
		.cAccumBits      = 0,
		.cAccumRedBits   = 0,
		.cAccumGreenBits = 0,
		.cAccumBlueBits  = 0,
		.cAccumAlphaBits = 0,
		.cDepthBits      = 16,
		.cStencilBits    = 0,
		.cAuxBuffers     = 0,
		.iLayerType      = PFD_MAIN_PLANE,
		.bReserved       = 0,
		.dwLayerMask     = 0,
		.dwVisibleMask   = 0,
		.dwDamageMask    = 0
	};

	if(!SetPixelFormat(graphics->window_device_context, ChoosePixelFormat(graphics->window_device_context, &pixel_format_descriptor), &pixel_format_descriptor))
	{
		//log_error("SetPixelFormat");
		goto error;
	}

	graphics->opengl_context = wglCreateContext(graphics->window_device_context);
	if(!graphics->opengl_context)
	{
		//log_error("wglCreateContext");
		goto error;
	}

	//if(!wglMakeCurrent(graphics->window_device_context, graphics->opengl_context))
	//{
		//log_error("wglMakeCurrent");
	//	goto error;
	//}

	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();

	//Rational_Number32 aspect_ratio;
	//aspect_ratio = (Rational_Number32)height / (Rational_Number32)width;
	//glScalef(1.0 * aspect_ratio, 1.0, 1.0);

	return;

error:
}


static void begin_draw_graphics(Graphics* graphics)
{
	wglMakeCurrent(graphics->window_device_context, graphics->opengl_context);
}


static void end_draw_graphics(Graphics* graphics)
{
	SwapBuffers(graphics->window_device_context);
}


static Boolean get_next_graphics_message (Graphics* graphics)
{
	Windows_Message message;
	Boolean         has_messages;

	has_messages = PeekMessageA(&message, graphics->window, 0, 0, REMOVE_MESSAGE_FROM_QUEUE);

	if(has_messages)
	{
		TranslateMessage(&message);
		DispatchMessageA(&message);

		switch(message.type)
		{
			case CREATE_MESSAGE: graphics->last_message.type = CREATE_GRAPHICS; break;
			case DESTROY_MESSAGE: graphics->last_message.type = DESTROY_GRAPHICS; break;

			default:
				graphics->last_message.type = UNKNOWN_GRAPHICS_MESSAGE;
		}
	}

	return has_messages;
}


static void deinitialize_graphics(Graphics* graphics)
{
	wglDeleteContext(graphics->opengl_context);
	ReleaseDC(graphics->window, graphics->window_device_context);
	DestroyWindow(graphics->window);
}


#endif


#endif//SYSTEM_GRAPHICS_INCLUDED
/*

#include <heap.c>


void main()
{
	Graphics  graphics;
	Graphics  graphics2;
	Number32* canvas;
	Number    i;

	initialize_graphics(&graphics, 0, 0, 320, 240);
	initialize_graphics(&graphics2, 320, 0, 320, 240);

	canvas = allocate_memory(320 * 240 * 4);

	for(i = 0; i < graphics.width * graphics.height; ++i)
		canvas[i] = (128 << 24) + 128;

	for(;;)
	{
		begin_draw_graphics(&graphics);
		{
			glClear(GL_COLOR_BUFFER_BIT);
			glDrawPixels(320, 240, GL_RGBA, GL_UNSIGNED_BYTE, canvas);
		}
		end_draw_graphics(&graphics);

		while(get_next_graphics_message(&graphics))
		{
			if(graphics.last_message.type == DESTROY_GRAPHICS)
				return;
		}


		begin_draw_graphics(&graphics2);
		{
			glClear(GL_COLOR_BUFFER_BIT);

			glColor3f(1, 0, 0);
			glBegin(GL_TRIANGLES);
				glVertex3f(0, 0, 0);
				glVertex3f(1, 0, 0);
				glVertex3f(1, 1, 0);
			glEnd();
		}
		end_draw_graphics(&graphics2);

		//ShowScrollBar(graphics2.window, SB_HORZ, 1);
		//ShowScrollBar(graphics2.window, SB_VERT, 1);

		while(get_next_graphics_message(&graphics2))
		{
			if(graphics2.last_message.type == DESTROY_GRAPHICS)
				deinitialize_graphics(&graphics2);
		}
	}

	deinitialize_graphics(&graphics);
	deinitialize_graphics(&graphics2);
}


#include <string.c>

void _start()
{
	Byte* utf8_command_line;

	utf8_command_line = get_utf8_argument(GetCommandLineA());
	print(utf8_command_line);
	free_memory(utf8_command_line);

	main();

	ExitProcess(0);
}*/