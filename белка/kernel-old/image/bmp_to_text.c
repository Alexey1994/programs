#include <file.c>


void main()
{
	FILE_READER(L"a.bmp")
	FILE_WRITER(L"b.txt")
		//bitmap header
		read_binary_Number16(&reader);//signature BM
		read_binary_Number32(&reader);//file size in bytes
		read_binary_Number32(&reader);//zero
		Number pixel_data_offset = read_binary_Number32(&reader);
		printf("%d\n", pixel_data_offset);

		//bitmap info
		Number version = read_binary_Number32(&reader);
		Number width = read_binary_Number32(&reader);
		Number height = read_binary_Number32(&reader);
		read_binary_Number16(&reader);//planes
		Number bits_per_pixel = read_binary_Number16(&reader);
		/*read_binary_Number32(&reader);//compression
		read_binary_Number32(&reader);//pixel data size
		read_binary_Number32(&reader);//pixels per meter width
		read_binary_Number32(&reader);//pixels per meter height
		read_binary_Number32(&reader);//size of cells in color table
		read_binary_Number32(&reader);//number of cells*/

		set_file_position(&reader, pixel_data_offset);

		Number i;
		for(i = 0; i < width * height; ++i)
		{
			write_Number(&writer, read_binary_Number8(&reader));
			write_null_terminated_bytes(&writer, ", ");
		}

		//printf("%d %d %d", width, height, bits_per_pixel);
		//print(_Number, width, 0);
	END_WRITER
	END_READER
}