#ifndef HTTP_INCLUDED
#define HTTP_INCLUDED


#include <system/network/client/TCP.c>
#include <system/network/server/TCP.c>
#include <reader.c>
#include <writer.c>
#include <data-structures/buffer.c>


#endif//HTTP_INCLUDED


typedef enum
{
	HTTP_GET    = 1,
	HTTP_POST   = 2,
	HTTP_PUT    = 3,
	HTTP_DELETE = 4
}
HTTP_Method;


typedef struct
{
	HTTP_Method method;
	Buffer      path;
}
HTTP_Request;


void read_HTTP_token(Reader* reader, Buffer* token)
{
	Byte character;

	for(;;)
	{
		character = get_reader_byte(reader);

		if(is_space_character(character))
			break;

		read_next_byte(reader);
		add_bytes_in_buffer_end(token, &character, 1);
	}
}


Boolean read_HTTP_Request(HTTP_Request* request, Reader* reader)
{
	request->method = 0;
	clear_buffer(&request->path);

	if(read_if(reader, "GET"))
		request->method = HTTP_GET;
	else if(read_if(reader, "POST"))
		request->method = HTTP_POST;
	else if(read_if(reader, "PUT"))
		request->method = HTTP_PUT;
	else if(read_if(reader, "DELETE"))
		request->method = HTTP_DELETE;
	else
		goto error;

	read_spaces(reader);
	read_HTTP_token(reader, &request->path);
	read_line(reader);

	while(!read_if(reader, "\r\n"))
		read_line(reader);

	return 1;

error:
	return 0;
}


void print_token(Buffer* token)
{
	Number i;

	for(i = 0; i < token->length; ++i)
		printf("%c", token->data[i]);
}


Boolean on_HTTP_request(
	Socket client_socket,
	Integer_Number (*read_from_socket)(Socket client_socket, Byte* bytes, Number number_of_bytes),
	Integer_Number (*write_in_socket)(Socket client_socket, Byte* bytes, Number number_of_bytes),
	Byte* arguments
)
{
	Reader       reader;
	HTTP_Request request;
	Writer       writer;

	initialize_reader(&reader, client_socket, read_from_socket);
	initialize_buffer(&request.path, 128);
	read_HTTP_Request(&request, &reader);

	print_token(&request.path);
	printf("\n");
	//Byte bytes[512]; read_bytes(&reader, bytes, 512); printf("%s\n", bytes);

	initialize_writer(&writer, client_socket, write_in_socket);
	write_null_terminated_bytes(
		&writer,
		"HTTP/1.1 200 Ok\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: 10\r\n"
		"\r\n"
		"<b>Hi</b>t"
	);
	deinitialize_writer(&writer);

	deinitialize_buffer(&request.path);
	deinitialize_reader(&reader);
}


Boolean start_HTTP_server(Address* address)
{
	return start_TCP_server(address, &on_HTTP_request, 0);
}


void main()
{
	Address address;

	//initialize_address(&address, "0.0.0.0", "80", TCP_PROTOCOL);
	initialize_address(&address, "127.0.0.1", "80", TCP_PROTOCOL);
	start_HTTP_server(&address);
}