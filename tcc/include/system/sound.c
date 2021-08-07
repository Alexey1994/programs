#ifndef SYSTEM_SOUND_INCLUDED
#define SYSTEM_SOUND_INCLUDED


#include <types.c>


typedef struct
{
	Integer_Number16 left;
	Integer_Number16 right;	
}
Sound_Data;


#ifdef __WIN32__

#include <system/Windows/winmm.c>
#include <system/memory.c>
#include <memory.c>


#define SOUND_BUFFER_SIZE 8820 // in bytes


typedef struct
{
	Byte*         device;
	Byte          buffer1_data[SOUND_BUFFER_SIZE];
	Byte          buffer2_data[SOUND_BUFFER_SIZE];
	Sound_Buffer  buffer1;
	Sound_Buffer  buffer2;
	Number        current_buffer_index;
	Boolean       is_playing;

	Byte*    source;
	Number (*read_bytes_from_source)(
		Byte* source,
		Sound_Data* sound_buffer,
		Number size_of_sound_buffer
	);
	void (*write_bytes_in_source)(
		Byte* source,
		Sound_Data* sound_buffer,
		Number size_of_sound_buffer
	);
}
Sound;


static stdcall void sound_out_message_handler(Byte* sound_device, Number32 message, Sound* sound, Number32* parameter1, Number32* parameter2)
{
	switch(message)
	{
		case MM_WOM_OPEN:
			break;

		case MM_WOM_DONE:
			if(sound->is_playing)
			{
				if(sound->current_buffer_index)
				{
					clear_bytes(sound->buffer1_data, sound->buffer1.buffer_length);
					sound->read_bytes_from_source(sound->source, sound->buffer1_data, sound->buffer1.buffer_length);
					waveOutWrite(sound_device, &sound->buffer1, sizeof(Sound_Buffer));
				}
				else
				{
					clear_bytes(sound->buffer2_data, sound->buffer2.buffer_length);
					sound->read_bytes_from_source(sound->source, sound->buffer2_data, sound->buffer2.buffer_length);
					waveOutWrite(sound_device, &sound->buffer2, sizeof(Sound_Buffer));
				}

				sound->current_buffer_index = !sound->current_buffer_index;
			}
			break;

		case MM_WOM_CLOSE:
			free_memory(sound);
			break;
	}
}

Sound* create_sound_out(
	Byte* source,
	Number (*read_bytes_from_source)(
		Byte* source,
		Sound_Data* sound_buffer,
		Number size_of_sound_buffer
	)
)
{
	Sound*       sound;
	Number32     status;
	Sound_Format sound_format;

	sound = allocate_memory(sizeof(Sound));

	sound->source                 = source;
	sound->read_bytes_from_source = read_bytes_from_source;
	sound->current_buffer_index   = 1;
	sound->is_playing             = 1;

	sound->buffer1.data           = sound->buffer1_data;
	sound->buffer1.buffer_length  = SOUND_BUFFER_SIZE;
	sound->buffer1.bytes_recorded = 0;
	sound->buffer1.flags          = 0;
	sound->buffer1.loops          = 0;

	sound->buffer2.data           = sound->buffer2_data;
	sound->buffer2.buffer_length  = SOUND_BUFFER_SIZE;
	sound->buffer2.bytes_recorded = 0;
	sound->buffer2.flags          = 0;
	sound->buffer2.loops          = 0;

	sound_format.format                       = PCM_WAVE_FORMAT;
	sound_format.number_of_channels           = 2;
	sound_format.samples_per_seconds          = 44100;
	sound_format.bits_per_sample              = 16;
	sound_format.block_align                  = sound_format.bits_per_sample / 8 * sound_format.number_of_channels;
	sound_format.bytes_per_seconds            = sound_format.samples_per_seconds * sound_format.block_align;
	sound_format.size_of_appended_information = 0;

	if(waveOutOpen(&sound->device, -1, &sound_format, &sound_out_message_handler, sound, CALLBACK_FUNCTION))
		goto error;

	if(waveOutPrepareHeader(sound->device, &sound->buffer1, sizeof(Sound_Buffer)))
	{
		waveOutClose(sound->device);
		goto error;
	}

	if(waveOutPrepareHeader(sound->device, &sound->buffer2, sizeof(Sound_Buffer)))
	{
		waveOutUnprepareHeader(sound->device, &sound->buffer1, sizeof(Sound_Buffer));
		waveOutClose(sound->device);
		goto error;
	}

	clear_bytes(sound->buffer1_data, sound->buffer1.buffer_length);
	read_bytes_from_source(source, sound->buffer1_data, SOUND_BUFFER_SIZE);
	waveOutWrite(sound->device, &sound->buffer1, sizeof(Sound_Buffer));

	clear_bytes(sound->buffer2_data, sound->buffer2.buffer_length);
	read_bytes_from_source(source, sound->buffer2_data, SOUND_BUFFER_SIZE);
	waveOutWrite(sound->device, &sound->buffer2, sizeof(Sound_Buffer));

	return sound;

error:
	free_memory(sound);
	return 0;
}


void destroy_sound_out(Sound* sound)
{
	sound->is_playing = 0;
	waveOutReset(sound->device);
	waveOutUnprepareHeader(sound->device, &sound->buffer1, sizeof(Sound_Buffer));
	waveOutUnprepareHeader(sound->device, &sound->buffer2, sizeof(Sound_Buffer));
	waveOutClose(sound->device);
}


static stdcall void sound_in_message_handler(Byte* sound_device, Number32 message, Sound* sound, Number32* parameter1, Number32* parameter2)
{
	switch(message)
	{
		case MM_WIM_OPEN:
			break;

		case MM_WIM_DATA:
			if(sound->is_playing)
			{
				if(sound->current_buffer_index)
				{
					sound->write_bytes_in_source(sound->source, sound->buffer1_data, sound->buffer1.buffer_length);
					waveInAddBuffer(sound->device, &sound->buffer1, sizeof(Sound_Buffer));
				}
				else
				{
					sound->write_bytes_in_source(sound->source, sound->buffer2_data, sound->buffer2.buffer_length);
					waveInAddBuffer(sound->device, &sound->buffer2, sizeof(Sound_Buffer));
				}

				sound->current_buffer_index = !sound->current_buffer_index;
			}
			break;

		case MM_WIM_CLOSE:
			free_memory(sound);
			break;
	}
}

Sound* create_sound_in(
	Byte* source,
	void (*write_bytes_in_source)(
		Byte* source,
		Sound_Data* sound_buffer,
		Number size_of_sound_buffer
	)
)
{
	Sound*       sound;
	Number32     status;
	Sound_Format sound_format;

	sound = allocate_memory(sizeof(Sound));

	sound->source                 = source;
	sound->write_bytes_in_source  = write_bytes_in_source;
	sound->current_buffer_index   = 1;
	sound->is_playing             = 1;

	sound->buffer1.data           = sound->buffer1_data;
	sound->buffer1.buffer_length  = SOUND_BUFFER_SIZE;
	sound->buffer1.bytes_recorded = 0;
	sound->buffer1.flags          = 0;
	sound->buffer1.loops          = 0;

	sound->buffer2.data           = sound->buffer2_data;
	sound->buffer2.buffer_length  = SOUND_BUFFER_SIZE;
	sound->buffer2.bytes_recorded = 0;
	sound->buffer2.flags          = 0;
	sound->buffer2.loops          = 0;

	sound_format.format                       = PCM_WAVE_FORMAT;
	sound_format.number_of_channels           = 2;
	sound_format.samples_per_seconds          = 44100;
	sound_format.bits_per_sample              = 16;
	sound_format.block_align                  = sound_format.bits_per_sample / 8 * sound_format.number_of_channels;
	sound_format.bytes_per_seconds            = sound_format.samples_per_seconds * sound_format.block_align;
	sound_format.size_of_appended_information = 0;

	if(waveInOpen(&sound->device, -1, &sound_format, &sound_in_message_handler, sound, CALLBACK_FUNCTION))
		goto error;

	if(waveInPrepareHeader(sound->device, &sound->buffer1, sizeof(Sound_Buffer)))
	{
		waveInClose(sound->device);
		goto error;
	}

	if(waveInPrepareHeader(sound->device, &sound->buffer2, sizeof(Sound_Buffer)))
	{
		waveInUnprepareHeader(sound->device, &sound->buffer1, sizeof(Sound_Buffer));
		waveInClose(sound->device);
		goto error;
	}

	clear_bytes(sound->buffer1_data, sound->buffer1.buffer_length);
	waveInAddBuffer(sound->device, &sound->buffer1, sizeof(Sound_Buffer));

	clear_bytes(sound->buffer2_data, sound->buffer2.buffer_length);
	waveInAddBuffer(sound->device, &sound->buffer2, sizeof(Sound_Buffer));

	waveInStart(sound->device);

	return sound;

error:
	free_memory(sound);
	return 0;
}


void destroy_sound_in(Sound* sound)
{
	sound->is_playing = 0;
	waveInStop(sound->device);
	waveInUnprepareHeader(sound->device, &sound->buffer1, sizeof(Sound_Buffer));
	waveInUnprepareHeader(sound->device, &sound->buffer2, sizeof(Sound_Buffer));
	waveInClose(sound->device);
}


#endif//__WIN32__


#endif//SYSTEM_SOUND_INCLUDED

/*
Number32 sound_data[SOUND_BUFFER_SIZE / 2];


Number read_sound_data(Number16* data, Number16* sound_buffer, Number size_of_sound_buffer)
{
	copy_bytes(sound_buffer, data, size_of_sound_buffer);
	return size_of_sound_buffer;
}


void main()
{
	Number i;
	for(i = 0; i < SOUND_BUFFER_SIZE / 2; ++i)
		((Number32*)((Byte*)sound_data + 2))[i] = i % 200 + 190;

	Sound* sound;
	
	sound = create_sound_out(sound_data, &read_sound_data);
	Sleep(1000);
	destroy_sound_out(sound);
}*/

/*
Number32 sound_data[SOUND_BUFFER_SIZE / 2];

Number write_sound_data(Sound* out, Number16* sound_buffer, Number size_of_sound_buffer)
{
	copy_bytes(sound_buffer, sound_data, size_of_sound_buffer);
	
	return size_of_sound_buffer;
}

void read_sound_data(Sound* out, Number16* sound_buffer, Number size_of_sound_buffer)
{
	copy_bytes(sound_data, sound_buffer, size_of_sound_buffer);
}


void main()
{
	Sound* in;
	Sound* out;

	out = create_sound_out(0, &write_sound_data);
	in = create_sound_in(0, &read_sound_data);

	Sleep(2000);

	destroy_sound_out(out);
	destroy_sound_in(in);
}*/


/*
#define NUMBER_OF_CHUNKS 64


Sound_Data sound_data_queue[NUMBER_OF_CHUNKS * SOUND_BUFFER_SIZE / sizeof(Sound_Data)];
Number sound_data_queue_start = 0;
Number sound_data_queue_end   = 0;

Number get_sound_queue_size()
{
	if(sound_data_queue_end <= sound_data_queue_start)
		return sound_data_queue_start - sound_data_queue_end;
	else
		return NUMBER_OF_CHUNKS - sound_data_queue_start + sound_data_queue_end;
}


#include <system/Windows/user32.c>
#include <system/Windows/gdi32.c>
#include <system/keyboard.c>


Byte*   window;
Boolean window_closed = 0;

Byte* background_color;
Byte* row_color;

stdcall Number32 update_window_state(Byte* window, Number32 message, Number32* parameter1, Integer_Number32* parameter2)
{
	switch(message)
	{
		case PAINT_MESSAGE:
		{
			Windows_Paint paint = {
				.erase_background = 1
			};
			Byte* paint_device = BeginPaint(window, &paint);
			{
				Number x;
				Number y = 256;
				Number width = 1;
				Integer_Number height;

				Windows_Rectangle rectangle = {0, 0, 512, 512};
				FillRect(paint_device, &rectangle, background_color);

				Number t = sound_data_queue_start;
				Number xx = 0;

				for(x = 0; x < 512; ++x)
				{
					height = ((sound_data_queue + t * (SOUND_BUFFER_SIZE / sizeof(Sound_Data)))[xx * 4 * NUMBER_OF_CHUNKS].left) / 128;

					if(x && !(x % (512 / NUMBER_OF_CHUNKS)))
					{
						xx = 0;
						++t;

						if(t >= NUMBER_OF_CHUNKS)
							t = 0;
					}
					else
						++xx;

					Windows_Rectangle rectangle = {x, y, x + width, y + height};
					FillRect(paint_device, &rectangle, row_color);
				}
			}
			EndPaint(window, &paint);
			break;
		}

		case DESTROY_MESSAGE:
			window_closed = 1;
			break;
	}

	return DefWindowProcW(window, message, parameter1, parameter2);
}

void create_window()
{
	Window_Class window_class = {
		.structure_size   = sizeof(Window_Class),
		.window_procedure = &update_window_state,
		.cursor           = LoadCursorA(0, DEFAULT_ARROW),
		.background       = CreateSolidBrush(255 + (255 << 8) + (255 << 16)),
		.class_name       = L"sound"
	};

	RegisterClassExW(&window_class);

	window = CreateWindowExW(
		WS_EX_DLGMODALFRAME,
		L"sound",
		L"sound",
		WS_OVERLAPPED | WS_SYSMENU,
		0, 0, 512, 512,
		0,
		0,
		0,
		0
	);

	ShowWindow(window, SHOW_DEFAULT_WINDOW);
}


Byte   received_byte;
Number bit_index_in_received_byte;
Number number_of_empty_data;

void print_received_data(Sound_Data* sound, Number number_of_sounds)
{
	Number i;

	for(i = 0; i < number_of_sounds; ++i)
	{
		if(sound[i].left > 256 || sound[i].left < -256)
		{
			if(sound[i].left > 0)
				received_byte = received_byte | (1 << bit_index_in_received_byte);
			else
				received_byte = received_byte & (0b11111111 >> (8 - bit_index_in_received_byte));

			++bit_index_in_received_byte;

			if(bit_index_in_received_byte == 8)
			{
				printf("%c", received_byte);
				bit_index_in_received_byte = 0;
				received_byte = 0;
			}
		}
		else
		{
			++number_of_empty_data;

			if(number_of_empty_data > 16)
				bit_index_in_received_byte = 0;	
		}
	}
}

void read_sound_data(Sound* out, Sound_Data* sound_buffer, Number size_of_sound_buffer)
{
	print_received_data(sound_buffer, size_of_sound_buffer / sizeof(Sound_Data));
	copy_bytes(sound_data_queue + sound_data_queue_end * (SOUND_BUFFER_SIZE / sizeof(Sound_Data)), sound_buffer, size_of_sound_buffer);

	++sound_data_queue_end;

	if(sound_data_queue_end >= NUMBER_OF_CHUNKS)
		sound_data_queue_end = 0;

	if(get_sound_queue_size > NUMBER_OF_CHUNKS)
	{
		++sound_data_queue_start;

		if(sound_data_queue_start >= NUMBER_OF_CHUNKS)
			sound_data_queue_start = 0;
	}

	RedrawWindow(window, 0, 0, RDW_INVALIDATE);
}


void main()
{
	Sound* in;

	background_color = CreateSolidBrush(0);
	row_color = CreateSolidBrush(255 << 8);
	create_window();

	bit_index_in_received_byte = 0;
	number_of_empty_data = 0;
	in = create_sound_in(0, &read_sound_data);

	Message message;

	while(!window_closed && GetMessageW(&message, 0, 0, 0))
	{
		TranslateMessage(&message);
		DispatchMessageW(&message);
	}

	destroy_sound_in(in);
}*/


#include <system/Windows/user32.c>
#include <system/Windows/gdi32.c>
#include <system/keyboard.c>


Byte*   window;
Boolean window_closed = 0;

Byte* background_color;
Byte* row_color;

Number pressed_key_code = 0;

stdcall Number32 update_window_state(Byte* window, Number32 message, Number32* parameter1, Integer_Number32* parameter2)
{
	switch(message)
	{
		case KEY_UP_MESSAGE:
		{
			pressed_key_code = parameter1;
			break;
		}

		case DESTROY_MESSAGE:
			window_closed = 1;
			break;
	}

	return DefWindowProcW(window, message, parameter1, parameter2);
}

void create_window()
{
	Window_Class window_class = {
		.structure_size   = sizeof(Window_Class),
		.window_procedure = &update_window_state,
		.cursor           = LoadCursorA(0, DEFAULT_ARROW),
		.background       = CreateSolidBrush(255 + (255 << 8) + (255 << 16)),
		.class_name       = L"sound"
	};

	RegisterClassExW(&window_class);

	window = CreateWindowExW(
		WS_EX_DLGMODALFRAME,
		L"sound",
		L"sound",
		WS_OVERLAPPED | WS_SYSMENU,
		0, 0, 512, 512,
		0,
		0,
		0,
		0
	);

	ShowWindow(window, SHOW_DEFAULT_WINDOW);
}


Number bit_index_in_sended_byte;


Number write_sound_data(Sound* out, Sound_Data* sound_buffer, Number size_of_sound_buffer)
{
	Number i;

	clear_bytes(sound_buffer, size_of_sound_buffer);

	if(pressed_key_code)
	{
		i = 0;

		while(bit_index_in_sended_byte < 8)
		{
			sound_buffer[i].left = (pressed_key_code & (1 << bit_index_in_sended_byte))
				? 2048
				: -2048;
			++i;
			++bit_index_in_sended_byte;
		}

		printf("send %c\n", pressed_key_code);
		bit_index_in_sended_byte = 0;
		pressed_key_code = 0;
	}

	return size_of_sound_buffer;
}


void main()
{
	Sound* out;

	background_color = CreateSolidBrush(0);
	row_color = CreateSolidBrush(255 << 8);
	create_window();

	bit_index_in_sended_byte = 0;
	out = create_sound_out(0, &write_sound_data);

	Message message;

	while(!window_closed && GetMessageW(&message, 0, 0, 0))
	{
		TranslateMessage(&message);
		DispatchMessageW(&message);
	}

	destroy_sound_out(out);
}