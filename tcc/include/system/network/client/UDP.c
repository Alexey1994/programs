#ifndef SYSTEM_NETWORK_CLIENT_UDP
#define SYSTEM_NETWORK_CLIENT_UDP


#include <system/network/address.c>


typedef struct
{
	Bit32    socket;
	Address* destination_address;
}
UDP_Client;


Boolean initialize_UDP_client   (UDP_Client* connection, Address* address);
void    deinitialize_UDP_client (UDP_Client* connection);

Integer_Number write_in_UDP_client  (UDP_Client* connection, Byte* bytes, Number number_of_bytes);
Integer_Number read_from_UDP_client (UDP_Client* connection, Byte* bytes, Number number_of_bytes);


#ifdef __WIN32__

Boolean initialize_UDP_client (UDP_Client* connection, Address* address)
{
	if(!initialize_Windows_sockets())
		goto error;

	connection->socket = socket(IPv4, SOCKET_DATAGRAMS, UDP_PROTOCOL);

	if(connection->socket == -1)
		goto error;

	connection->destination_address = address;

	return 1;

error:
	return 0;
}


void deinitialize_UDP_client (UDP_Client* connection)
{
	closesocket(connection->socket);
}


Integer_Number write_in_UDP_client (UDP_Client* connection, Byte* bytes, Number number_of_bytes)
{
	return sendto(connection->socket, bytes, number_of_bytes, 0, connection->destination_address, sizeof(Address));
}


Integer_Number read_from_UDP_client (UDP_Client* connection, Byte* bytes, Number number_of_bytes)
{
	return recvfrom(connection->socket, bytes, number_of_bytes, 0, 0, 0);
}

#endif//__WIN32__


#endif//SYSTEM_NETWORK_CLIENT_UDP

/*
void main()
{
	Address address;
	Byte    buffer[1024];
	Bit32   length;

	if(!initialize_address(&address, "localhost", "80", UDP_PROTOCOL))
	{
		printf("address not found\n");
		return;
	}

	UDP_Client udp;
	initialize_UDP_client(&udp, &address);
	write_in_UDP_client(&udp, "Hi, server", 11);
	length = read_from_UDP_client(&udp, buffer, 1024);
	printf("response %d bytes: %s", length, buffer);
	deinitialize_UDP_client(&udp);
}*/