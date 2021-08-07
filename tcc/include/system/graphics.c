#ifndef SYSTEM_GRAPHICS_INCLUDED
#define SYSTEM_GRAPHICS_INCLUDED


#include <types.c>

#ifdef __WIN32__
# include <system/Windows/gdi32.c>
# include <system/Windows/user32.c>
# include <system/Windows/opengl32.c>
# include <system/Windows/glu32.c>
#endif


#define MAX_NUMBER_OF_MESSAGES 16


typedef struct
{
	Bit32  width;
	Bit32  height;
	Bit32* data;
	Bit8*  system_graphics;

	Number  number_of_messages;
	Message messages[MAX_NUMBER_OF_MESSAGES];
}
Graphics;


//void initialize_graphics   (Graphics* graphics, Bit32 width, Bit32 height);
void draw_graphics         (Graphics* graphics);
void deinitialize_graphics (Graphics* graphics);


#ifdef __WIN32__


static stdcall Number32 update_window_state(Byte* window, Message_Type type, Number32* parameters_1, Integer_Number32* parameters_2)
{
	switch(type)
	{
		case CLOSE_MESSAGE:
			PostQuitMessage(0);
			return 0;

		case DESTROY_MESSAGE:
			return 0;
	}

	return DefWindowProcA(window, type, parameters_1, parameters_2);
}


void initialize_graphics(Graphics* graphics, Number32 width, Number32 height)
{
	Number32 status;

	graphics->width              = width;
	graphics->height             = height;
	graphics->number_of_messages = 0;

	Window_Class window_class = {
		.structure_size   = sizeof(Window_Class),
		.style            = OWN_WINDOW_CONTEXT,
		.window_procedure = &update_window_state,
		.icon             = LoadIconA(0, DEFAULT_ICON),
		.cursor           = LoadCursorA(0, DEFAULT_ARROW),
		.class_name       = "Main window class",
		.small_icon       = LoadIconA(0, DEFAULT_ICON)
	};

	status = RegisterClassExA(&window_class);

	if(!status)
		goto error;

	Byte* window = CreateWindowExA(
		0,
		"Main window class",
		"Main",
		WS_OVERLAPPED,
		0,
		0,
		width,
		height,
		0,
		0,
		window_class.instance,
		0
	);

	if(!window)
	{
		//log_error("CreateWindowExA")
		goto error;
	}

	//SetWindowLongA(window, USER_DATA, graphics);
	ShowWindow(window, SHOW_DEFAULT_WINDOW);

	graphics->system_graphics = GetDC(window);

	Windows_Pixel_Format_Descriptor pixel_format_descriptor = {
		.size            = sizeof(Windows_Pixel_Format_Descriptor),
		.nVersion        = 1,
		.dwFlags         = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		.iPixelType      = PFD_TYPE_RGBA,
		.cColorBits      = 24,
		.cDepthBits      = 16,
		.iLayerType      = PFD_MAIN_PLANE
	};

	if(!SetPixelFormat(graphics->system_graphics, ChoosePixelFormat(graphics->system_graphics, &pixel_format_descriptor), &pixel_format_descriptor))
	{
		//log_error("SetPixelFormat");
		goto error;
	}

	Byte* opengl_context = wglCreateContext(graphics->system_graphics);
	if(!opengl_context)
	{
		//log_error("wglCreateContext");
		goto error;
	}

	if(!wglMakeCurrent(graphics->system_graphics, opengl_context))
	{
		//log_error("wglMakeCurrent");
		goto error;
	}

	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();

	Rational_Number32 aspect_ratio;
	aspect_ratio = (Rational_Number32)height / (Rational_Number32)width;
	glScalef(1.0 * aspect_ratio, 1.0, 1.0);

	return;

error:
}


void update_messages(Graphics* graphics)
{
	Message* message;
	
	graphics->number_of_messages = 0;

	for(;;)
	{
		message = graphics->messages + graphics->number_of_messages;

		if(graphics->number_of_messages >= MAX_NUMBER_OF_MESSAGES)
			break;

		if(!PeekMessageA(message, 0, 0, 0, REMOVE_MESSAGE_FROM_QUEUE))
			break;

		TranslateMessage(message);
		DispatchMessageA(message);

		++graphics->number_of_messages;
	}
}


void draw_graphics(Graphics* graphics)
{
	SwapBuffers(graphics->system_graphics);
}

#endif


Message* find_message(Graphics* graphics, Message_Type type)
{
	Number i;

	for(i = 0; i < graphics->number_of_messages && i < MAX_NUMBER_OF_MESSAGES; ++i)
		if(graphics->messages[i].type == type)
			return graphics->messages + i;

	return 0;
}


void print_messages(Graphics* graphics)
{
	Number i;

	for(i = 0; i < graphics->number_of_messages && i < MAX_NUMBER_OF_MESSAGES; ++i)
		printf("%d ", graphics->messages[i].type);

	if(graphics->number_of_messages)
		printf("\n");
}


#endif//SYSTEM_GRAPHICS_INCLUDED

/*
void test_2D()
{
	Graphics graphics;
	Number32* canvas = malloc(1440 * 900 * 4);

	initialize_graphics(&graphics, 1440, 900);

	for(;;)
	{
		Number i;
		for(i = 0; i < graphics.width * graphics.height; ++i)
			canvas[i] = (128 << 24) + 128;

		glClear(GL_COLOR_BUFFER_BIT);
		glRasterPos2f(-1, -1);
		glDrawPixels(1440, 900, GL_RGBA, GL_UNSIGNED_BYTE, canvas);

		draw_graphics(&graphics);
	}
}


void test_3D()
{
	Graphics graphics;

	initialize_graphics(&graphics, 1440, 900);

	for(;;)
	{
		glClear(GL_COLOR_BUFFER_BIT);

		glColor3f(1, 0, 0);
		glBegin(GL_TRIANGLES);
			glVertex3f(0, 0, 0);
			glVertex3f(1, 0, 0);
			glVertex3f(1, 1, 0);
		glEnd();

		draw_graphics(&graphics);
	}
}


void main()
{
	test_2D();
	//test_3D();
}*/