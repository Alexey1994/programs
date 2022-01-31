#ifndef SYSTEM_CAMERA_INCLUDED
#define SYSTEM_CAMERA_INCLUDED


#include <types.c>


static void print_camera_info(Number32 device_index);
static void create_camera(
	Number32 camera_index,
	void (*get_frame)(
		Byte*    frame,
		Number32 width,
		Number32 height,
		Number16 bits_per_pixel,
		Byte*    arguments
	),
	Byte* arguments
);


#ifdef __WIN32__

#include <Windows/avicap32.c>
#include <Windows/user32.c>
#include <Windows/gdi32.c>


typedef struct
{
	Number32  width;
	Number32  height;
	Number16  bits_per_pixel;

	void (*get_frame)(
		Byte* frame,
		Number32 width,
		Number32 height,
		Number16 bits_per_pixel,
		Byte* arguments
	);
	Byte* arguments;
}
Camera_User_data;


static void print_camera_info(Number32 device_index)
{
	Bit8 name[100];
	Bit8 version[100];

	if(capGetDriverDescriptionA(device_index, name, 100, version, 100))
	{
		log(name, ", ", version);
	}
	else
	{
		log("camera device not found");
	}
}

static stdcall Number32 handle_preview_frame_camera(Byte* window, Video_Frame* frame)
{

}

static stdcall Number32 handle_camera(Byte* window, Video_Frame* frame)
{
	Camera_User_data* user_data;

	user_data = SendMessageA(window, WM_CAP_GET_USER_DATA, 0, 0);
	user_data->get_frame(
		frame->data,
		user_data->width,
		user_data->height,
		user_data->bits_per_pixel,
		user_data->arguments
	);

	return 0;
}

static void create_camera(
	Number32 camera_index,
	void (*get_frame)(
		Byte* frame,
		Number32 width,
		Number32 height,
		Number16 bits_per_pixel,
		Byte* arguments
	),
	Byte* arguments
)
{
	Bit8*            window;
	Bitmap_Info      camera_parameters;
	Camera_User_data user_data;

	window = capCreateCaptureWindowA("Camera preview", 0x10000000, 0, 0, 1440, 900, 0, 0); //capCreateCaptureWindowA("Camera preview", /*POPUP_WINDOW_STYLE*/0x10000000 | 0x00800000, 0, 0, 640, 480, 0, 0);
	//ShowWindow(window, SHOW_MAXIMIZED_WINDOW);
	//ShowWindow(window, 5);

	if(SendMessageA(window, WM_CAP_DRIVER_CONNECT, camera_index, 0) > 0)
	{
		log("Camera connected");
	}
	else
	{
		log("Camera not connected");
	}

	//SendMessageA(window, WM_CAP_DLG_VIDEOFORMAT, 0, 0);
	//SendMessageA(window, WM_CAP_GET_VIDEOFORMAT, sizeof(Bitmap_Info), &camera_parameters);

	//SendMessageA(window, WM_CAP_DLG_VIDEOCOMPRESSION, 0, 0);

	//SendMessageA(window, WM_CAP_SET_PREVIEWRATE, 10, 0);
	SendMessageA(window, WM_CAP_SET_SCALE, 1, 0);
	//SendMessageA(window, WM_CAP_SET_OVERLAY, 1, 0);

	user_data.width          = camera_parameters.header.width;
	user_data.height         = camera_parameters.header.height;
	user_data.bits_per_pixel = camera_parameters.header.bit_count;
	user_data.get_frame      = get_frame;
	user_data.arguments      = arguments;
	SendMessageA(window, WM_CAP_SET_USER_DATA, 0, &user_data);

	//SendMessageA(window, WM_CAP_SET_CALLBACK_VIDEOSTREAM, 0, &handle_camera);
	SendMessageA(window, WM_CAP_SET_CALLBACK_FRAME, 0, &handle_camera);

	for(;;)
	{
		Windows_Message message;

		while(PeekMessageA(&message, 0, 0, 0, REMOVE_MESSAGE_FROM_QUEUE))
		{
			TranslateMessage(&message);
			DispatchMessageA(&message);
		}

		Sleep(10);

		{
			SendMessageA(window, WM_CAP_GRAB_FRAME, 0, 0);
			//SendMessageA(window, WM_CAP_SEQUENCE_NOFILE, 0, 0);
		}
	}
}

#endif//__WIN32__


#endif//SYSTEM_CAMERA_INCLUDED


void get_camera_frame(Byte* frame, Number32 width, Number32 height, Number16 bits_per_pixel, Byte* arguments)
{
	
	//cycle(0, width, 1)
	//	Byte a = frame->data[i];
	//end
	//printf("%d\n", width*height*2);
}


void main()
{
	print_camera_info(0);
	create_camera(0, &get_camera_frame, 0);
}