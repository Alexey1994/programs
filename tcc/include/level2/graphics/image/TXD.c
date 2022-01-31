#ifndef GRAPHICS_IMAGE_TXD_INCLUDED
#define GRAPHICS_IMAGE_TXD_INCLUDED


#include <reader.c>
#include <memory.c>
#include <graphics.c>


typedef struct
{
	Number width;
	Number height;
	Number bytes_per_pixel;
	Byte*  pixels;
}
TXD_Image;


Boolean read_TXD_image(Reader* reader, TXD_Image* image)
{
	/*Byte            compressed[]   = {0,0,10,0,0,0,0,0,0,0,0,0};
	Byte            uncompressed[] = {0,0,2,0,0,0,0,0,0,0,0,0};
	Boolean         is_compressed;
	Integer_Number  number_of_pixels;
	Number          number_of_bytes;
	Number8         number_of_chunks;
	Byte*           pixels;
	Byte            repetable_pixels[4];

	if(read_if_bytes(reader, compressed, sizeof(compressed)))
		is_compressed = 1;
	else if(read_if_bytes(reader, uncompressed, sizeof(uncompressed)))
		is_compressed = 0;
	else
		goto error;

	image->width = read_binary_Number16(reader);
	image->height = read_binary_Number16(reader);
	image->bytes_per_pixel = read_binary_Number8(reader) / 8;
	read_binary_Number8(reader);

	number_of_pixels = image->width * image->height;
	number_of_bytes = number_of_pixels * image->bytes_per_pixel;
	image->pixels = allocate_memory(number_of_bytes);

	if(is_compressed)
	{
		pixels = image->pixels;

		while(number_of_pixels > 0)
		{
			number_of_chunks = read_binary_Number8(reader);

			if(number_of_chunks < 128)
			{
				++number_of_chunks;
				read_bytes(reader, pixels, number_of_chunks * image->bytes_per_pixel);
				pixels += number_of_chunks * image->bytes_per_pixel;
				number_of_pixels -= number_of_chunks;
			}
			else
			{
				number_of_chunks -= 127;
				read_bytes(reader, repetable_pixels, image->bytes_per_pixel);

				while(number_of_chunks)
				{
					copy_bytes(pixels, repetable_pixels, image->bytes_per_pixel);
					pixels += image->bytes_per_pixel;
					--number_of_pixels;
					--number_of_chunks;
				}
			}
		}
	}
	else
		read_bytes(reader, image->pixels, number_of_bytes);*/

	Number32 type;
	Number32 size;
	Number32 stamp;

	Number32 number_of_textures;

	Number32 platform_id;
	Number32 texture_settings;
	Byte     name[32];
	Byte     mask_name[32];

	type = read_binary_Number32(reader);

	if(type != 0x16)
		goto error;

	size = read_binary_Number32(reader);
	stamp = read_binary_Number32(reader);

	number_of_textures = read_binary_Number32(reader);

	//while(number_of_textures)
	{
		platform_id = read_binary_Number32(reader);
		texture_settings = read_binary_Number32(reader);
		read_bytes(reader, name, 32);
		read_bytes(reader, mask_name, 32);

		--number_of_textures;
	}

	print("n", number_of_textures);
	//print("s", mask_name);

	//print("n", size);

	return 1;

error:
print("e")
	return 0;
}


void deinitialize_TGA_image(TXD_Image* image)
{
	free_memory(image->pixels);
}


Number32 bind_TXD_Image_to_texture(TXD_Image* image)
{
	Number32 texture_id;

	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGBA,
		image->width,
		image->height,
		0,
		image->bytes_per_pixel == 3 ? GL_BGR : GL_BGRA,
		GL_UNSIGNED_BYTE,
		image->pixels
	);

	return texture_id;
}


#endif//GRAPHICS_IMAGE_TXD_INCLUDED


#include <file.c>


void main()
{
	Reader    reader;
	TXD_Image image;

	initialize_file_reader(&reader, "a.txd");
	read_TXD_image(&reader, &image);
	deinitialize_reader(&reader);

/*
	Graphics             graphics;
	Rational_Number32    aspect_ratio;

	initialize_graphics(&graphics, 0, 0, 1440, 900);

	begin_draw_graphics(&graphics);
	aspect_ratio = 900.0f / 1440.0f;
	glScalef(aspect_ratio, 1, 1);

	glEnable(GL_TEXTURE_2D);
	bind_TXD_Image_to_texture(&image);
	end_draw_graphics(&graphics);

	for(;;)
	{
		begin_draw_graphics(&graphics);

		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-1, -1, 0);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(-1,  1, 0);
			glTexCoord2f(1.0f, 1.0f); glVertex3f( 1,  1, 0);
			glTexCoord2f(1.0f, 0.0f); glVertex3f( 1, -1, 0);
		glEnd();

		end_draw_graphics(&graphics);

		while(get_next_graphics_message(&graphics))
		{
			if(graphics.last_message.type == DESTROY_GRAPHICS)
				return;
		}
	}*/
}