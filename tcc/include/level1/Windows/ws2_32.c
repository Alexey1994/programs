#ifndef SYSTEM_WINDOWS_WS2_32_INCLUDED
#define SYSTEM_WINDOWS_WS2_32_INCLUDED


#include <types.c>


//=============================== WSA ================================


typedef struct
{
    Bit16  version;
    Bit16  high_version;
    Bit8   description[257];
    Bit8   system_status[129];
    Bit32  maximum_sockets;
    Bit32  maximum_UDP_size;
    Bit8*  vendor_info;
}
Windows_Sockets;

import Bit32 WSAStartup(Bit16 version, Windows_Sockets* sockets);
import Bit32 WSACleanup();
import Bit32 WSAGetLastError();


//=============================== Address Info =======================


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
Windows_Address;


typedef struct
{
    Bit32                 flags;
    Bit32                 family;
    Bit32                 socket_type;
    Bit32                 protocol;
    Bit32                 address_length;
    Bit8*                 name;
    Windows_Address*      address;
    struct Address_Info*  next;
}
Address_Info;

import Address_Info* getaddrinfo  (Bit8* host, Bit8* port, Address_Info* hints, Address_Info** result);
import void          freeaddrinfo (Address_Info* address_info);


//=============================== Socket =============================

typedef Integer_Number32 Socket;

import Socket   socket      (Bit32 family, Bit32 type, Bit32 protocol);
import Bit32    closesocket (Socket socket);
import Bit32    connect     (Socket socket, Windows_Address* address, Number32 address_length);
import Bit32    bind        (Socket socket, Windows_Address* address, Number32 address_length);
import Number32 send        (Socket socket, Byte* buffer, Number32 buffer_length, Bit32 flags);
import Number32 sendto      (Socket socket, Byte* buffer, Number32 buffer_length, Bit32 flags, Address* destination_address, Number32 destination_address_length);
import Number32 recv        (Socket socket, Byte* buffer, Number32 buffer_length, Bit32 flags);
import Number32 recvfrom    (Socket socket, Byte* buffer, Number32 buffer_length, Bit32 flags, Address* destination_address, Number32* destination_address_length);
import Socket   accept      (Socket socket, Windows_Address* address, Number32 address_length);
import Bit32    listen      (Socket socket, Number32 maximum_pending_connections);
import Bit32    ioctlsocket (Socket socket, Bit32 command, Bit32* arguments);


#endif //SYSTEM_WINDOWS_WS2_32_INCLUDED