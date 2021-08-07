#ifndef GRAPHICS_IMAGE_TGA_INCLUDED
#define GRAPHICS_IMAGE_TGA_INCLUDED


#include <reader.c>
#include <memory.c>
#include <system/graphics.c>


typedef struct
{
	Number width;
	Number height;
	Number bytes_per_pixel;
	Byte*  pixels;
}
TGA_Image;


Boolean read_TGA_image(Reader* reader, TGA_Image* image)
{
	Byte            compressed[]   = {0,0,10,0,0,0,0,0,0,0,0,0};
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
		read_bytes(reader, image->pixels, number_of_bytes);

	return 1;

error:
	return 0;
}


void deinitialize_TGA_image(TGA_Image* image)
{
	free_memory(image->pixels);
}


Number32 bind_TGA_Image_to_texture(TGA_Image* image)
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


#endif//GRAPHICS_IMAGE_TGA_INCLUDED

/*
#include <file.c>


void main()
{
	Reader    reader;
	TGA_Image image;

	initialize_file_reader(&reader, L"a.tga");
	read_TGA_image(&reader, &image);
	deinitialize_reader(&reader);


	Graphics             graphics;
	Rational_Number32    aspect_ratio;

	initialize_graphics(&graphics, 1920, 1080);

	aspect_ratio = 1080.0f / 1920.0f;
	glScalef(aspect_ratio, 1, 1);

	glEnable(GL_TEXTURE_2D);
	bind_TGA_Image_to_texture(&image);

	for(;;)
	{
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-1, -1, 0);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(-1,  1, 0);
			glTexCoord2f(1.0f, 1.0f); glVertex3f( 1,  1, 0);
			glTexCoord2f(1.0f, 0.0f); glVertex3f( 1, -1, 0);
		glEnd();

		draw_graphics(&graphics);
	}
}*/