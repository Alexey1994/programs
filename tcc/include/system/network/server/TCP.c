#ifndef SYSTEM_NETWORK_SERVER_TCP_INCLUDED
#define SYSTEM_NETWORK_SERVER_TCP_INCLUDED


#include <system/network/address.c>


Boolean start_TCP_server(
	Address* address,

	Boolean (*on_request)(
		Socket client_socket,
		Integer_Number (*read_from_socket)(Socket client_socket, Byte* bytes, Number number_of_bytes),
		Integer_Number (*write_in_socket)(Socket client_socket, Byte* bytes, Number number_of_bytes),
		Byte* arguments
	),

	Byte* arguments
);


#ifdef __WIN32__

static Integer_Number read_from_socket(Socket socket, Byte* bytes, Number number_of_bytes)
{
	return recv(socket, bytes, number_of_bytes, 0);
}


static Integer_Number write_in_socket(Socket socket, Byte* bytes, Number number_of_bytes)
{
	return send(socket, bytes, number_of_bytes, 0);
}


Boolean start_TCP_server(
	Address* address,

	Boolean (*on_request)(
		Socket client_socket,
		Integer_Number (*read_from_socket)(Socket client_socket, Byte* bytes, Number number_of_bytes),
		Integer_Number (*write_in_socket)(Socket client_socket, Byte* bytes, Number number_of_bytes),
		Byte* arguments
	),

	Byte* arguments
)
{
	Socket server_socket;

	if(!initialize_Windows_sockets())
		goto error;

	server_socket = socket(IPv4, SOCKET_STREAM, TCP_PROTOCOL);

	if(server_socket == -1)
	{
		//print_error("socket error\n");
		goto error;
	}

	if(bind(server_socket, address, sizeof(Address)) == -1)
	{
		//print_error("bind error\n");
		goto error;
	}

	if(listen(server_socket, 10000))
	{
		//print_error("listen error\n");
		goto error;
	}

	for(;;)
	{
		Socket client_socket = accept(server_socket, 0, 0);

		if(client_socket < 0)
		{
			//sleep_thread(1);
			continue;
		}

		if(!on_request(client_socket, &read_from_socket, &write_in_socket, arguments))
		{
			closesocket(client_socket);
			break;
		}

		closesocket(client_socket);
	}

	return 1;

error:
	return 0;
}

#endif//__WIN32__


#endif//SYSTEM_NETWORK_SERVER_TCP_INCLUDED


Boolean listen_server(
	Socket client_socket,
	Integer_Number (*read_from_socket)(Socket client_socket, Byte* bytes, Number number_of_bytes),
	Integer_Number (*write_in_socket)(Socket client_socket, Byte* bytes, Number number_of_bytes),
	Byte* arguments
)
{
	printf("request %d\n", arguments);

	return 1;
}


void main()
{
	Address server_address;

	initialize_address(&server_address, "127.0.0.1", "80", TCP_PROTOCOL);
	start_TCP_server(&server_address, &listen_server, 123);
}