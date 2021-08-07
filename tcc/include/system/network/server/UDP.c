#ifndef SYSTEM_NETWORK_SERVER_UDP_INCLUDED
#define SYSTEM_NETWORK_SERVER_UDP_INCLUDED


#include <system/network/address.c>


typedef struct
{
	Socket  socket;
	Address client_address;
}
UDP_Server;


Boolean start_UDP_server(
	Address* address,

	Boolean (*on_request)(
		UDP_Server* server,
		Integer_Number (*read)(UDP_Server* server, Byte* bytes, Number number_of_bytes),
		Integer_Number (*write)(UDP_Server* server, Byte* bytes, Number number_of_bytes),
		Byte* arguments
	),

	Byte* arguments
);


#ifdef __WIN32__

static Integer_Number read(UDP_Server* server, Byte* bytes, Number number_of_bytes)
{
	Number32 client_address_length;
	return recvfrom(server->socket, bytes, number_of_bytes, 0, &server->client_address, &client_address_length);
}


static Integer_Number write(UDP_Server* server, Byte* bytes, Number number_of_bytes)
{
	return sendto(server->socket, bytes, number_of_bytes, 0, &server->client_address, sizeof(Address));
}


Boolean start_UDP_server(
	Address* address,

	Boolean (*on_request)(
		UDP_Server* server,
		Integer_Number (*read)(UDP_Server* server, Byte* bytes, Number number_of_bytes),
		Integer_Number (*write)(UDP_Server* server, Byte* bytes, Number number_of_bytes),
		Byte* arguments
	),

	Byte* arguments
)
{
	UDP_Server server;

	if(!initialize_Windows_sockets())
		goto error;

	server.socket = socket(IPv4, SOCKET_DATAGRAMS, UDP_PROTOCOL);

	if(server.socket == -1)
	{
		//print_error("socket error\n");
		goto error;
	}

	if(bind(server.socket, address, sizeof(Address)) == -1)
	{
		//print_error("bind error\n");
		goto error;
	}

	for(;;)
	{
		if(!on_request(&server, &read, &write, arguments))
			break;
	}

	return 1;

error:
	return 0;
}

#endif//__WIN32__


#endif//SYSTEM_NETWORK_SERVER_UDP_INCLUDED

/*
Boolean listen_server(
	UDP_Server* server,
	Integer_Number (*read)(UDP_Server* server, Byte* bytes, Number number_of_bytes),
	Integer_Number (*write)(UDP_Server* server, Byte* bytes, Number number_of_bytes),
	Byte* arguments
)
{
	Byte buffer[512];
	Integer_Number bytes_readed;

	bytes_readed = read(server, buffer, 512);

	if(bytes_readed != -1)
	{
		printf("request %d, %d bytes: %s\n", arguments, bytes_readed, buffer);
		write(server, "Hi", 3);
	}

	return 1;
}


void main()
{
	Address server_address;

	initialize_address(&server_address, "127.0.0.1", "80", UDP_PROTOCOL);
	start_UDP_server(&server_address, &listen_server, 123);
}*/