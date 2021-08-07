#ifndef SYSTEM_NETWORK_ADDRESS_INCLUDED
#define SYSTEM_NETWORK_ADDRESS_INCLUDED


#include <types.c>


typedef enum
{
	ANY_INTERNET_PROTOCOL = 0,
	IPv4                  = 2,
	IPv6                  = 23
}
Network_Family;


typedef enum
{
	SOCKET_STREAM    = 1,
	SOCKET_DATAGRAMS = 2
}
Socket_Type;


typedef enum
{
	TCP_PROTOCOL = 6,
	UDP_PROTOCOL = 17
}
Network_Protocol;


typedef struct
{
    Bit8  family;
    Bit8  zero;
    Bit16 port;

    union
    {
        Bit32 ip_v4;
        Bit32 ip_v6_flow_info;
    };

    Bit8 ip_v6[16];

    union
    {
        Bit32 ip_v6_scope_id;

        union
        {
            struct
            {
                Bit32 zone: 28;
                Bit32 level: 4;
            };

            Bit32 value;
        };
    };
}
Address;


Boolean initialize_address (Address* address, Byte* host, Byte* port, Network_Protocol protocol);


#ifdef __WIN32__

#include <system/Windows/ws2_32.c>
#include <memory.c>


static Windows_Sockets windows_sockets = {0};


Boolean initialize_Windows_sockets()
{
    if(!windows_sockets.version)
        return !WSAStartup(0x0202, &windows_sockets);

    return 1;
}


void deinitialize_Windows_sockets()
{
    if(windows_sockets.version)
        WSACleanup();

    windows_sockets.version = 0;
}


Boolean initialize_address (Address* address, Byte* host, Byte* port, Network_Protocol protocol)
{
    Address_Info* address_info;
    Address_Info  hints;

    if(!initialize_Windows_sockets())
        goto error;

    clear_bytes(&hints, sizeof(Address_Info));

    if(protocol == UDP_PROTOCOL)
    {
        hints.family      = IPv4;
        hints.socket_type = SOCKET_DATAGRAMS;
        hints.protocol    = UDP_PROTOCOL;
    }
    else if(protocol == TCP_PROTOCOL)
    {
        hints.family      = IPv4;
        hints.socket_type = SOCKET_STREAM;
        hints.protocol    = TCP_PROTOCOL;
    }
    else
        goto error;

    if(getaddrinfo(host, port, &hints, &address_info))
        goto error;

    clear_bytes(address, sizeof(Address));
    copy_bytes(address, address_info->address, address_info->address_length);

    freeaddrinfo(address_info);

    return 1;

error:
    return 0;
}


#endif//__WIN32__


#endif//SYSTEM_NETWORK_ADDRESS_INCLUDED

/*
void main()
{
    Address address;
    Boolean address_initialized;

    address_initialized = initialize_address(&address, "vk.com", "80", TCP_PROTOCOL);
    //log(write_N_32, address_initialized);
    printf("%d", address_initialized);
}*/