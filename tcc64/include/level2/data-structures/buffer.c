#ifndef BUFFER_DATA_STRUCTURES_INCLUDED
#define BUFFER_DATA_STRUCTURES_INCLUDED


#include <system/memory.c>
#include <memory.c>


typedef struct
{
	Number  begin_index;
	Number  length;
	Number  reserve;
	Byte*   data;
}
Buffer;


void initialize_buffer(Buffer* buffer, Number reserve)
{
	buffer->data        = allocate_memory(reserve);
	buffer->reserve     = reserve;
	buffer->length      = 0;
	buffer->begin_index = 0;
}


void deinitialize_buffer(Buffer* buffer)
{
	free_memory(buffer->data);
}


void reserve_buffer_memory(Buffer* buffer, Number length)
{
	Number old_reserve;
	Byte*  old_data;
	Number new_data_length;

	new_data_length = buffer->length + length;

	if(new_data_length > buffer->reserve)
	{
		old_reserve = buffer->reserve;
		buffer->reserve = new_data_length * 2;
		old_data = buffer->data;
		buffer->data = allocate_memory(buffer->reserve);

		copy_bytes(buffer->data, old_data + buffer->begin_index, old_reserve - buffer->begin_index);
		copy_bytes(buffer->data + (old_reserve - buffer->begin_index), old_data, buffer->begin_index);

		buffer->begin_index = 0;

		free_memory(old_data);
	}
}


Number add_bytes_in_buffer_end(Buffer* buffer, Byte* bytes, Number bytes_length)
{
	Number data_index;
	Byte*  data;
	Number i;

	reserve_buffer_memory(buffer, bytes_length);

	data_index = buffer->begin_index + buffer->length;

	if(data_index >= buffer->reserve)
		data_index -= buffer->reserve;

	data = buffer->data + data_index;

	for(i = 0; i < bytes_length; ++i)
	{
		if(i == buffer->reserve - buffer->begin_index)
			data = buffer->data;

		*data = bytes[i];
		++data;
	}

	buffer->length += bytes_length;

	return bytes_length;
}


Number remove_bytes_from_buffer_end(Buffer* buffer, Byte* bytes, Number bytes_length)
{
	Number data_index;
	Byte*  data;
	Number bytes_readed;
	Number i;

	bytes_readed = 0;
	data_index = buffer->begin_index + buffer->length;

	if(data_index >= buffer->reserve)
		data_index -= buffer->reserve;

	data = buffer->data + buffer->length - 1;

	for(i = 0; i < bytes_length; ++i)
	{
		if(!buffer->length)
			break;

		bytes[bytes_length - 1 - i] = *data;

		if(data == buffer->data)
			data = buffer->data + buffer->reserve - 1;
		else
			--data;

		--buffer->length;
		++bytes_readed;
	}

	return bytes_readed;
}


Number add_bytes_in_buffer_begin(Buffer* buffer, Byte* bytes, Number bytes_length)
{
	Byte*  data;
	Number i;

	reserve_buffer_memory(buffer, bytes_length);

	data = buffer->data + buffer->begin_index;

	for(i = 0; i < bytes_length; ++i)
	{
		if(data == buffer->data)
		{
			data = buffer->data + buffer->reserve;
			buffer->begin_index = buffer->reserve;
		}

		--data;
		--buffer->begin_index;

		*data = bytes[bytes_length - 1 - i];
	}

	buffer->length += bytes_length;

	return bytes_length;
}


Number remove_bytes_from_buffer_begin(Buffer* buffer, Byte* bytes, Number bytes_length)
{
	Byte*  data;
	Number bytes_readed;
	Number i;

	bytes_readed = 0;
	data = buffer->data + buffer->begin_index;

	for(i = 0; i < bytes_length; ++i) {
		if(!buffer->length)
			break;

		bytes[i] = *data;
		++buffer->begin_index;

		if(buffer->begin_index == buffer->reserve)
		{
			buffer->begin_index = 0;
			data = buffer->begin_index;
		}
		else
			++data;

		--buffer->length;
		++bytes_readed;
	}

	return bytes_readed;
}


void get_buffer_bytes(Buffer* buffer, Number offset, Byte* bytes, Number bytes_length)
{
	Number data_index;
	Byte*  data;
	Number i;

	data_index = buffer->begin_index + offset;

	if(data_index >= buffer->reserve)
		data_index -= buffer->reserve;

	data = buffer->data + data_index;

	for(i = 0; i < bytes_length; ++i)
	{
		if(i == buffer->reserve - buffer->begin_index - offset)
			data = buffer->data;

		bytes[i] = *data;
		++data;
	}
}


void set_buffer_bytes(Buffer* buffer, Number offset, Byte* bytes, Number bytes_length)
{
	Number data_index;
	Byte*  data;
	Number i;

	data_index = buffer->begin_index + offset;

	if(data_index >= buffer->reserve)
		data_index -= buffer->reserve;

	data = buffer->data + data_index;

	for(i = 0; i < bytes_length; ++i)
	{
		if(i == buffer->reserve - buffer->begin_index - offset)
			data = buffer->data;

		*data = bytes[i];
		++data;
	}
}


void add_buffer_in_buffer_end(Buffer* buffer, Buffer* tail)
{
	Number data_index;
	Byte*  data;
	Number i;

	reserve_buffer_memory(buffer, tail->length);

	data_index = buffer->begin_index + buffer->length;

	if(data_index >= buffer->reserve)
		data_index -= buffer->reserve;

	data = buffer->data + data_index;

	for(i = 0; i < tail->length; ++i)
	{
		if(i == buffer->reserve - buffer->begin_index)
			data = buffer->data;

		Byte tail_byte;
		get_buffer_bytes(tail, i, &tail_byte, 1);
		*data = tail_byte;
		++data;
	}

	buffer->length += tail->length;
}


void add_buffer_in_buffer_begin(Buffer* buffer, Buffer* tail)
{
	Byte*  data;
	Number i;

	reserve_buffer_memory(buffer, tail->length);

	data = buffer->data + buffer->begin_index;

	for(i = 0; i < tail->length; ++i)
	{
		if(data == buffer->data)
		{
			data = buffer->data + buffer->reserve;
			buffer->begin_index = buffer->reserve;
		}

		--data;
		--buffer->begin_index;

		Byte tail_byte;
		get_buffer_bytes(tail, tail->length - 1 - i, &tail_byte, 1);
		*data = tail_byte;
	}

	buffer->length += tail->length;
}


void clear_buffer(Buffer* buffer)
{
	buffer->length      = 0;
	buffer->begin_index = 0;
}


Boolean buffer_empty(Buffer* buffer)
{
	return !buffer->length;
}


#define BUFFER(_buffer_context, reserve)\
{\
	Buffer _buffer_context;\
	initialize_buffer(&_buffer_context, (reserve));
#define END_BUFFER(_buffer_context)\
	deinitialize_buffer(&_buffer_context);\
}


#define add_in_buffer_begin(_buffer_context, _buffer_element_type, _buffer_element)\
{\
	_buffer_element_type _buffer_element_copy = _buffer_element;\
	add_bytes_in_buffer_begin(_buffer_context, &_buffer_element_copy, sizeof(_buffer_element_type));\
}


#define add_in_buffer_end(_buffer_context, _buffer_element_type, _buffer_element)\
{\
	_buffer_element_type _buffer_element_copy = _buffer_element;\
	add_bytes_in_buffer_end(_buffer_context, &_buffer_element_copy, sizeof(_buffer_element_type));\
}


#define for_each_buffer_element(_buffer_context, _buffer_element_type, _buffer_element_name)\
{\
	_buffer_element_type _buffer_element_name;\
	Number _buffer_element_name##_index;\
	Number _buffer_element_name##_i;\
	Number _buffer_element_name##_j;\
	Number _size_of_##_buffer_element_name = sizeof(_buffer_element_type);\
	\
	for(\
		_buffer_element_name##_i = (_buffer_context)->begin_index, _buffer_element_name##_j = 0;\
		_buffer_element_name##_j < (_buffer_context)->length;\
		++_buffer_element_name##_i, ++_buffer_element_name##_j\
	)\
	{\
		if(_buffer_element_name##_i == (_buffer_context)->reserve)\
			_buffer_element_name##_i = 0;\
		\
		((Byte*)&(_buffer_element_name))[(_buffer_element_name##_j % _size_of_##_buffer_element_name)] = (_buffer_context)->data[_buffer_element_name##_i];\
		\
		if(_buffer_element_name##_j % sizeof(_buffer_element_type) == sizeof(_buffer_element_type) - 1)\
		{\

#define end_for_each_buffer_element\
		}\
	}\
}


#define BUFFER_READER(buffer)\
{\
	Reader reader;\
	initialize_reader(&reader, buffer, &remove_bytes_from_buffer_begin);\
	reader.end_of_data = &buffer_empty;

#define END_BUFFER_READER\
	deinitialize_reader(&reader);\
}


#define BUFFER_WRITER(buffer)\
{\
	Writer writer;\
	initialize_writer(&writer, buffer, &add_bytes_in_buffer_end);

#define END_BUFFER_WRITER\
	deinitialize_writer(&writer);\
}


#define BUFFER_RW()\
{\
	Buffer buffer;\
	initialize_buffer(&buffer, 128);\
\
	Reader reader;\
	initialize_reader(&reader, &buffer, &remove_bytes_from_buffer_begin);\
	reader.end_of_data = &buffer_empty;\
\
	Writer writer;\
	initialize_writer(&writer, &buffer, &add_bytes_in_buffer_end);\

#define END_BUFFER_RW\
	deinitialize_writer(&writer);\
	deinitialize_reader(&reader);\
	deinitialize_buffer(&buffer);\
}


#endif//BUFFER_DATA_STRUCTURES_INCLUDED

/*
void main()
{
	Byte bytes[20] = {};

	BUFFER(buffer, 1)
	{
		add_bytes_in_buffer_end(&buffer, "wol", 3);
		add_bytes_in_buffer_begin(&buffer, "d", 1);
		add_bytes_in_buffer_begin(&buffer, "fox", 3);
		remove_bytes_from_buffer_end(&buffer, bytes, 2);
		remove_bytes_from_buffer_begin(&buffer, bytes, 2);

		set_buffer_bytes(&buffer, 1, "HI", 2);

		for_each_buffer_element(&buffer, Byte, character)
			printf("%c", character);
		end_for_each_buffer_element

		get_buffer_bytes(&buffer, 1, bytes, 2);
		printf("\n%c%c", bytes[0], bytes[1]);
	}
	END_BUFFER(buffer)
}*/