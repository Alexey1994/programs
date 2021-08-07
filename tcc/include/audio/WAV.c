#ifndef WAV_AUDIO_INCLUDED
#define WAV_AUDIO_INCLUDED


#include <reader.c>


typedef struct
{
	Number32 data_size;
}
WAV;


Byte* read_WAV(WAV* wav, Reader* reader)
{
    Number32 i;
    Number32 size;
    Number32 subchunk_size;
    Number16 format;
    Number16 num_channels;
    Number32 frequency;
    Number32 byte_rate;
    Number16 block_align;
    Number16 bits_per_sample;

    wav->data_size = 0;

    if(!read_if(reader, "RIFF"))
        return 0;

    size = read_binary_Number32(reader);

    if(!read_if(reader, "WAVEfmt "))
        return 0;

    subchunk_size   = read_binary_Number32(reader);
    format          = read_binary_Number16(reader);
    num_channels    = read_binary_Number16(reader);
    frequency       = read_binary_Number32(reader);
    byte_rate       = read_binary_Number32(reader);
    block_align     = read_binary_Number16(reader);
    bits_per_sample = read_binary_Number16(reader);

    while(!end_of_reader(reader) && !read_if(reader, "data"))
    {
        read_next_byte(reader);
        read_next_byte(reader);
        read_next_byte(reader);
        read_next_byte(reader);
        
        subchunk_size = read_binary_Number32(reader);

        for(i = 0; i < subchunk_size; ++i)
            read_next_byte(reader);
    }

    wav->data_size = read_binary_Number32(reader);
}


#endif//WAV_AUDIO_INCLUDED

/*
#include <file.c>
#include <system/sound.c>
#include <log.c>


void main ()
{
    Sound* sound;
	WAV    wav;
	Reader reader;

	for(;;)
	{
		initialize_file_reader(&reader, L"a.wav");
		if(!read_WAV(&wav, &reader))
		{
			log_error("read WAV error");
			return 1;
		}
        sound = create_sound_out(&reader, &read_bytes);
        Sleep(15000);
        destroy_sound_out(sound);
        printf("next\n");
		deinitialize_reader(&reader);
	}
}*/