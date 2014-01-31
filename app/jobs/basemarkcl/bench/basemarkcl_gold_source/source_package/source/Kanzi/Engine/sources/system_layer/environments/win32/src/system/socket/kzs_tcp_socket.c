/**
* \file 
* Socket for reading and writing bytes via TCP.
* 
* Copyright 2009-2011 by Rightware. All rights reserved.
*/
/* TODO: Relative include for own header */
#include <system/socket/kzs_tcp_socket.h>

#include <system/wrappers/kzs_memory.h>
#include <system/kzs_error_codes.h>
#include <system/thread/kzs_thread.h>
#include <system/wrappers/kzs_string.h>

#include <windows.h>
#include <WinDef.h>

#define KZ_TCP_SOCKET_DEBUG 0 /* TODO: KZS_TCP... */
#if KZ_TCP_SOCKET_DEBUG
#include <stdio.h>
#endif


/** Number of attempts to connect to TCP socket until fail. */
#define KZ_TCP_SOCKET_CONNECTION_ATTEMPT_COUNT 5 /* TODO: Change to 1 */


struct KzsTCPSocket /* TODO: Document */
{
    SOCKET client; /* TODO: Combine client and socketHandle. Create a flag to indicate if it is server or client socket, if needed. */
    SOCKET server;
    SOCKET socketHandle;
    kzUint port;
    struct KzsThread* connectThread;
    kzBool connected;
    struct sockaddr_in socketAddress;
};

/* 
    At the moment, only one client connection is possible. 
    To allow multiple clients, we should store the socket handles returned by accept() for each client connection.
*/
static kzsError kzsTCPSocketServerConnectThread(void* data) /* TODO: _internal */
{
    kzsError result;
    struct KzsTCPSocket* tcpSocket = (struct KzsTCPSocket*)data;
    tcpSocket->socketHandle = accept(tcpSocket->server, NULL, NULL);
    if(tcpSocket->socketHandle == INVALID_SOCKET)
    {
        #if KZ_TCP_SOCKET_DEBUG
        printf("kzsTCPSocketServerConnectThread(): Windows Sockets operation accept() FAILED: %d \n", WSAGetLastError());
        #endif
        kzsErrorThrow(KZS_ERROR_SOCKET_OPERATION_FAILED, "Error in kzsTCPSocketConnectThread");
    }
    else
    {
        #if KZ_TCP_SOCKET_DEBUG
        printf("kzsTCPSocketConnectThread(): Server Connection Accepted\n");
        #endif
    }

    /* No longer need server socket */
    result = (kzsError)closesocket(tcpSocket->server);
    if(result == (kzsError)SOCKET_ERROR)
    {
        #if KZ_TCP_SOCKET_DEBUG
        printf("kzsTCPSocketServerConnectThread(): Windows Sockets operation closesocket() FAILED: %d SERVER\n", WSAGetLastError());
        #endif
        kzsErrorThrow(KZS_ERROR_SOCKET_OPERATION_FAILED, "Error in kzsTCPSocketDelete"); /* TODO: Make sure the error is handled somewhere, as this happens in another thread. */
    }
    tcpSocket->server = INVALID_SOCKET;

    tcpSocket->connected = KZ_TRUE;

	/* TODO: Call the connect-handler here. */
    kzsSuccess();
}

static kzsError kzsTCPSocketClientConnectThread(void* data)
{
    kzsError result;
    struct KzsTCPSocket* tcpSocket = (struct KzsTCPSocket*)data;
    /* Connect to server. */
    result = (kzsError)connect(tcpSocket->client, (SOCKADDR*) &tcpSocket->socketAddress, sizeof(tcpSocket->socketAddress));
    if(result == (kzsError)SOCKET_ERROR)
    {
        #if KZ_TCP_SOCKET_DEBUG
        printf("kzsTCPSocketClientConnectThread(): Windows Sockets operation connect() FAILED: %d \n", WSAGetLastError());
        #endif
        kzsErrorThrow(KZS_ERROR_SOCKET_OPERATION_FAILED, "Error in kzsTCPSocketClientConnectThread"); /* TODO: Better error message */
    }
    else
    {
        #if KZ_TCP_SOCKET_DEBUG
        printf("kzsTCPSocketClientConnectThread(): Client Connected\n");
        #endif
    }
    tcpSocket->connected = KZ_TRUE;
    kzsSuccess();
}

kzsError kzsTCPSocketCreateServer(kzString inetAddress, kzUint port, struct KzsTCPSocket** out_tcpSocket)
{
    kzsError result;
    struct KzsTCPSocket* tcpSocket;
    WSADATA wsaData;

    /* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h */
    WORD wVersionRequested = MAKEWORD(2, 2);

    tcpSocket = kzsMalloc(sizeof(*tcpSocket));
    kzsErrorTest(tcpSocket != KZ_NULL, KZS_ERROR_OUT_OF_MEMORY, "Out of memory while creating tcp socket");
    tcpSocket->socketHandle = INVALID_SOCKET;
    tcpSocket->client = INVALID_SOCKET;
    tcpSocket->server = INVALID_SOCKET;
    tcpSocket->port = port;
    tcpSocket->connectThread = KZ_NULL;
    tcpSocket->connected = KZ_FALSE;

	/* TODO: Extract function from the common code of server and client */
    result = (kzsError)WSAStartup(wVersionRequested, &wsaData); /* TODO: Do not use kzsError for windows errors */
    if(result != (kzsError)0) 
    {
        #if KZ_TCP_SOCKET_DEBUG /* TODO: Align left */
        printf("kzsTCPSocketCreateServer(): WSAStartup failed(%d)\n", result);
        #endif
        *out_tcpSocket = tcpSocket; /* TODO: Assign to output parameter as the last line before kzsSuccess */
        kzsErrorThrow(KZS_ERROR_SOCKET_OPERATION_FAILED, "Error in kzsTCPSocketCreate");
    }
    else if(LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) 
    {
        #if KZ_TCP_SOCKET_DEBUG
        printf("kzsTCPSocketCreateServer(): Could not find a usable version of Winsock.dll version(%d, %d)\n", LOBYTE(wsaData.wVersion), HIBYTE(wsaData.wVersion));
        #endif
        result = (kzsError)WSACleanup();
        if(result == (kzsError)SOCKET_ERROR)
        {
            #if KZ_TCP_SOCKET_DEBUG
            printf("kzsTCPSocketCreateServer(): Windows Sockets operation WSACleanup() FAILED: %d \n", WSAGetLastError());
            #endif
            *out_tcpSocket = tcpSocket;
            kzsErrorThrow(KZS_ERROR_SOCKET_OPERATION_FAILED, "Error in kzsTCPSocketCreate");
        }
    }
    else
    {
        #if KZ_TCP_SOCKET_DEBUG
        printf("kzsTCPSocketCreateServer(): The Winsock.dll version(%d, %d) was found. \n", LOBYTE(wsaData.wVersion), HIBYTE(wsaData.wVersion));
        #endif

        tcpSocket->socketAddress.sin_family = AF_INET;
        if(kzsStrcmp(inetAddress, "") == 0)
        {
            tcpSocket->socketAddress.sin_addr.s_addr = INADDR_ANY;
        }
        else
        {
            tcpSocket->socketAddress.sin_addr.s_addr = inet_addr(inetAddress);
        }

        tcpSocket->socketAddress.sin_port = htons((u_short)tcpSocket->port);
        tcpSocket->server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); /* AF_INET implies IPv4 */
        if(tcpSocket->server == INVALID_SOCKET) 
        {
            #if KZ_TCP_SOCKET_DEBUG
            printf("kzsTCPSocketCreateServer(): Windows Sockets operation socket() FAILED: %d \n", WSAGetLastError());
            #endif
            *out_tcpSocket = tcpSocket;
            kzsErrorThrow(KZS_ERROR_SOCKET_OPERATION_FAILED, "Error in kzsTCPSocketCreate");
        }

        result = (kzsError)bind(tcpSocket->server, (SOCKADDR*)&tcpSocket->socketAddress, sizeof(tcpSocket->socketAddress));
        if(result == (kzsError)SOCKET_ERROR)
        {
            #if KZ_TCP_SOCKET_DEBUG
            printf("kzsTCPSocketCreateServer(): Windows Sockets operation bind() FAILED: %d \n", WSAGetLastError());
            #endif
            *out_tcpSocket = tcpSocket;
            kzsErrorThrow(KZS_ERROR_SOCKET_OPERATION_FAILED, "Error in kzsTCPSocketCreate");
        }

        result = (kzsError)listen(tcpSocket->server, KZ_TCP_SOCKET_CONNECTION_ATTEMPT_COUNT);
        if(result == (kzsError)SOCKET_ERROR)
        {
            #if KZ_TCP_SOCKET_DEBUG
            printf("kzsTCPSocketCreateServer(): Windows Sockets operation listen() FAILED: %d \n", WSAGetLastError());
            #endif
            *out_tcpSocket = tcpSocket;
            kzsErrorThrow(KZS_ERROR_SOCKET_OPERATION_FAILED, "Error in kzsTCPSocketCreate");
        }

        #if KZ_TCP_SOCKET_DEBUG
        printf("kzsTCPSocketCreate(): Listening on socket...\n");
        #endif

        tcpSocket->connected = KZ_FALSE;
        result = kzsThreadCreate(kzsTCPSocketServerConnectThread, tcpSocket, KZ_TRUE, &tcpSocket->connectThread); 
        kzsErrorForward(result);

        *out_tcpSocket = tcpSocket;
    }
    kzsSuccess();
}

kzsError kzsTCPSocketCreateClient(kzString inetAddress, kzUint port, struct KzsTCPSocket** out_tcpSocket)
{
    kzsError result;
    struct KzsTCPSocket* tcpSocket;
    WSADATA wsaData;

    /* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h */
    WORD wVersionRequested = MAKEWORD(2, 2);

    tcpSocket = kzsMalloc(sizeof(*tcpSocket));
    kzsErrorTest(tcpSocket != KZ_NULL, KZS_ERROR_OUT_OF_MEMORY, "Out of memory while creating tcp socket");
    tcpSocket->socketHandle = INVALID_SOCKET;
    tcpSocket->client = INVALID_SOCKET;
    tcpSocket->server = INVALID_SOCKET;
    tcpSocket->port = port;
    tcpSocket->connectThread = KZ_NULL;
    tcpSocket->connected = KZ_FALSE;

    result = (kzsError)WSAStartup(wVersionRequested, &wsaData);
    if(result != (kzsError)0) 
    {
        #if KZ_TCP_SOCKET_DEBUG
        printf("kzsTCPSocketCreateClient(): WSAStartup failed(%d)\n", result);
        #endif
        *out_tcpSocket = tcpSocket;
        kzsErrorThrow(KZS_ERROR_SOCKET_OPERATION_FAILED, "Error in kzsTCPSocketCreateClient");
    }
    else if(LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) 
    {
        #if KZ_TCP_SOCKET_DEBUG
        printf("kzsTCPSocketCreateClient(): Could not find a usable version of Winsock.dll version(%d, %d)\n", LOBYTE(wsaData.wVersion), HIBYTE(wsaData.wVersion));
        #endif
        result = (kzsError)WSACleanup();
        if(result == (kzsError)SOCKET_ERROR)
        {
            #if KZ_TCP_SOCKET_DEBUG
            printf("kzsTCPSocketCreateClient(): Windows Sockets operation WSACleanup() FAILED: %d \n", WSAGetLastError());
            #endif
            *out_tcpSocket = tcpSocket;
            kzsErrorThrow(KZS_ERROR_SOCKET_OPERATION_FAILED, "Error in kzsTCPSocketCreateClient");
        }
    }
    else
    {
        #if KZ_TCP_SOCKET_DEBUG
        printf("kzsTCPSocketCreateClient(): The Winsock.dll version(%d, %d) was found. \n", LOBYTE(wsaData.wVersion), HIBYTE(wsaData.wVersion));
        #endif

        tcpSocket->socketAddress.sin_family = AF_INET;

        if(kzsStrcmp(inetAddress, "") == 0)
        {
            tcpSocket->socketAddress.sin_addr.s_addr = INADDR_ANY;
        }
        else
        {
            tcpSocket->socketAddress.sin_addr.s_addr = inet_addr(inetAddress);
        }

        tcpSocket->socketAddress.sin_port = htons((u_short)tcpSocket->port);
        tcpSocket->client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if(tcpSocket->client == INVALID_SOCKET) 
        {
            #if KZ_TCP_SOCKET_DEBUG
            printf("kzsTCPSocketCreateClient(): Windows Sockets operation socket() FAILED: %d \n", WSAGetLastError());
            #endif
            *out_tcpSocket = tcpSocket;
            kzsErrorThrow(KZS_ERROR_SOCKET_OPERATION_FAILED, "Error in kzsTCPSocketCreateClient");
        }

        tcpSocket->connected = KZ_FALSE;
        result = kzsThreadCreate(kzsTCPSocketClientConnectThread, tcpSocket, KZ_TRUE, &tcpSocket->connectThread); 
        kzsErrorForward(result);

        *out_tcpSocket = tcpSocket;
    }
    kzsSuccess();
}

kzsError kzsTCPSocketDelete(struct KzsTCPSocket* tcpSocket)
{
    kzsError result;
    if(tcpSocket->connected)
    {
        if(tcpSocket->client != INVALID_SOCKET) /* TODO: combine common code */
        {
            result = (kzsError)shutdown(tcpSocket->client, 2); /* TODO: Use define instead of 2. */
            if(result == (kzsError)SOCKET_ERROR)
            {
                #if KZ_TCP_SOCKET_DEBUG
                printf("kzsTCPSocketDelete(): Windows Sockets operation shutdown() FAILED: %d\n", WSAGetLastError());
                #endif
                kzsErrorThrow(KZS_ERROR_SOCKET_OPERATION_FAILED, "Error in kzsTCPSocketDelete");
            }

            result = (kzsError)closesocket(tcpSocket->client);
            if(result == (kzsError)SOCKET_ERROR)
            {
                #if KZ_TCP_SOCKET_DEBUG
                printf("kzsTCPSocketDelete(): Windows Sockets operation closesocket() FAILED: %d CLIENT\n", WSAGetLastError());
                #endif
                kzsErrorThrow(KZS_ERROR_SOCKET_OPERATION_FAILED, "Error in kzsTCPSocketDelete");
            }
            tcpSocket->client = INVALID_SOCKET;

            #if KZ_TCP_SOCKET_DEBUG
            printf("kzsTCPSocketDelete(): CLIENT DONE\n");
            #endif
        }
        else
        {
            result = (kzsError)shutdown(tcpSocket->socketHandle, 2);
            if(result == (kzsError)SOCKET_ERROR)
            {
                #if KZ_TCP_SOCKET_DEBUG
                printf("kzsTCPSocketDelete(): Windows Sockets operation shutdown() FAILED: %d\n", WSAGetLastError());
                #endif
                kzsErrorThrow(KZS_ERROR_SOCKET_OPERATION_FAILED, "Error in kzsTCPSocketDelete");
            }

            result = (kzsError)closesocket(tcpSocket->socketHandle);
            if(result == (kzsError)SOCKET_ERROR)
            {
                #if KZ_TCP_SOCKET_DEBUG
                printf("kzsTCPSocketDelete(): Windows Sockets operation closesocket() FAILED: %d SERVER\n", WSAGetLastError());
                #endif
                kzsErrorThrow(KZS_ERROR_SOCKET_OPERATION_FAILED, "Error in kzsTCPSocketDelete");
            }
            tcpSocket->socketHandle = INVALID_SOCKET;

            #if KZ_TCP_SOCKET_DEBUG
            printf("kzsTCPSocketDelete(): SERVER DONE\n");
            #endif
        }
    }

    result = (kzsError)WSACleanup();
    if(result == (kzsError)SOCKET_ERROR)
    {
        #if KZ_TCP_SOCKET_DEBUG
        printf("kzsTCPSocketDelete(): Windows Sockets operation WSACleanup() FAILED: %d \n", WSAGetLastError());
        #endif
        kzsErrorThrow(KZS_ERROR_SOCKET_OPERATION_FAILED, "Error in kzsTCPSocketDelete");
    }
    #if KZ_TCP_SOCKET_DEBUG
    printf("kzsTCPSocketDelete(): DONE\n");
    #endif

    kzsFree(tcpSocket);
    kzsSuccess();
}

kzUint kzsTCPSocketReadBytes(const struct KzsTCPSocket* tcpSocket, kzUint byteCount, kzByte* out_bytes)
{
    kzInt bytesRead;
    if(tcpSocket->client != INVALID_SOCKET)
    {
        bytesRead = (kzInt)recv((tcpSocket->client), (kzChar*)out_bytes, byteCount, 0);
    }
    else
    {
        bytesRead = (kzInt)recv((tcpSocket->socketHandle), (kzChar*)out_bytes, byteCount, 0);
    }

    if(bytesRead == SOCKET_ERROR)
    {
		/* TODO: Check the result of WSAGetLastError and react accordingly */
        #if KZ_TCP_SOCKET_DEBUG
        printf("kzsTCPSocketReadBytes(): Windows Sockets operation recv() FAILED: %d \n", WSAGetLastError());
        #endif
        /* Could not read any more bytes, but don't treat this situation as an error. */
        bytesRead = 0;
    }
    return bytesRead;
}

kzUint kzsTCPSocketWriteBytes(const struct KzsTCPSocket* tcpSocket, kzUint byteCount, const kzByte* bytes)
{
    kzInt bytesWritten;
    if(tcpSocket->client != INVALID_SOCKET)
    {
        bytesWritten = (kzInt)send((tcpSocket->client), (kzChar*)bytes, byteCount, 0);
    }
    else
    {
        bytesWritten = (kzInt)send((tcpSocket->socketHandle), (kzChar*)bytes, byteCount, 0);
    }

    if(bytesWritten == SOCKET_ERROR)
    {
		/* TODO: Check the result of WSAGetLastError and react accordingly */
        #if KZ_TCP_SOCKET_DEBUG
        printf("kzsTCPSocketWriteBytes(): Windows Sockets operation send() FAILED: %d \n", WSAGetLastError());
        #endif
        bytesWritten = 0;
    }
    return bytesWritten;
}

kzBool kzsTCPSocketIsConnected(const struct KzsTCPSocket* tcpSocket)
{
    return tcpSocket->connected;
}

