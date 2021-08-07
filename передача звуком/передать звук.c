int e = 'А';
int f = 'Б';

#include <memory.c>
#include <system/sound.c>


Number16 sound_data[SOUND_BUFFER_LENGTH / 2] = {0};


Number read_sound_data(Number16* data, Number16* sound_buffer, Number size_of_sound_buffer)
{
	clear_bytes(sound_data, sizeof(sound_data));
	sound_data[0] = sound_data[1] = 'P';
	sound_data[2] = sound_data[3] = 'I';
	sound_data[4] = sound_data[5] = 'V';
	sound_data[6] = sound_data[7] = 'E';
	sound_data[8] = sound_data[9] = 'T';

	//copy_bytes(sound_buffer, data, size_of_sound_buffer);
	return size_of_sound_buffer;
}


void main()
{
	//Number i;
	//for(i = 0; i < SOUND_BUFFER_LENGTH / 4; ++i)
	//	sound_data[i * 2] = i;//((Number32*)((Byte*)sound_data + 2))[i] = i % 200 + 190;

	initialize_sound_device(sound_data, &read_sound_data);

	for(;;);
}