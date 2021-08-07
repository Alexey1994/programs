#ifndef SYSTEM_NETWORK_CLIENT_TCP_INCLUDED
#define SYSTEM_NETWORK_CLIENT_TCP_INCLUDED


#include <system/network/address.c>


typedef struct
{
	Bit32 socket;
}
TCP_Client;


Boolean initialize_TCP_client   (TCP_Client* connection, Address* address);
void    deinitialize_TCP_client (TCP_Client* connection);

Integer_Number write_in_TCP_client  (TCP_Client* connection, Byte* bytes, Number32 number_of_bytes);
Integer_Number read_from_TCP_client (TCP_Client* connection, Byte* bytes, Number32 number_of_bytes);


#ifdef __WIN32__


Boolean initialize_TCP_client (TCP_Client* connection, Address* address)
{
	if(!initialize_Windows_sockets())
		goto error;

	connection->socket = socket(IPv4, SOCKET_STREAM, TCP_PROTOCOL);

	if(connection->socket == -1)
		goto error;

	if(connect(connection->socket, address, sizeof(Address)))
	{
		closesocket(connection->socket);
		goto error;
	}

	return 1;

error:
	return 0;
}


void deinitialize_TCP_client (TCP_Client* connection)
{
	closesocket(connection->socket);
}


Integer_Number write_in_TCP_client (TCP_Client* connection, Byte* bytes, Number32 number_of_bytes)
{
	return send(connection->socket, bytes, number_of_bytes, 0);
}


Integer_Number read_from_TCP_client (TCP_Client* connection, Byte* bytes, Number32 number_of_bytes)
{
	return recv(connection->socket, bytes, number_of_bytes, 0);
}


#endif//__WIN32__


#endif//SYSTEM_NETWORK_CLIENT_TCP_INCLUDED

/*
void main()
{
	Address  address;
	Byte     buffer[1024];
	Number32 length;


	if(!initialize_address(&address, "vk.com", "80", TCP_PROTOCOL))
	{
		printf("address not found\n");
		return;
	}

	TCP_Client tcp;
	initialize_TCP_client(&tcp, &address);

	Byte request[] =
		"GET / HTTP 1.1\r\n"
		"Host: vk.com\r\n"
		"Connection: close\r\n\r\n";

	snprintf(buffer, 1024, request);

	write_in_TCP_client(&tcp, buffer, sizeof(request) - 1);
	length = read_from_TCP_client(&tcp, buffer, 1024);
	printf("%d bytes: %s", length, buffer);
}*/