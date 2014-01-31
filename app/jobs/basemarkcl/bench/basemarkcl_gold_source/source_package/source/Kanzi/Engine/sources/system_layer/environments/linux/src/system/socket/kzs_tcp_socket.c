/**
* \file 
* Socket for reading and writing bytes via TCP.
* 
* Copyright 2009-2010 by Rightware. All rights reserved.
*/

#include <system/socket/kzs_tcp_socket.h>
#include <system/wrappers/kzs_memory.h>
#include <system/kzs_error_codes.h>
#include <system/thread/kzs_thread.h>
#include <system/wrappers/kzs_string.h>

#include <sys/types.h> 
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define KZ_TCP_SOCKET_DEBUG 0
#if KZ_TCP_SOCKET_DEBUG
#include <stdio.h>
#endif


typedef int SOCKET;


/** Number of attempts to connect to TCP socket until fail. */
#define KZ_TCP_SOCKET_CONNECTION_ATTEMPT_COUNT 5
#define TCP_SOCKET_ERROR -1


struct KzsTCPSocket
{
    SOCKET client;
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
static kzsError kzsTCPSocketServerConnectThread(void* data)
{
    kzsError result;
    struct KzsTCPSocket* tcpSocket = (struct KzsTCPSocket*)data;
    tcpSocket->socketHandle = accept(tcpSocket->server, NULL, NULL);
    if(tcpSocket->socketHandle == TCP_SOCKET_ERROR)
    {
        #if KZ_TCP_SOCKET_DEBUG
        printf("kzsTCPSocketServerConnectThread(): Sockets operation accept() FAILED\n");
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
    result = close(tcpSocket->server);
    if(result == TCP_SOCKET_ERROR)
    {
        #if KZ_TCP_SOCKET_DEBUG
        printf("kzsTCPSocketServerConnectThread(): Sockets operation close() FAILED: SERVER\n");
        #endif
        kzsErrorThrow(KZS_ERROR_SOCKET_OPERATION_FAILED, "Error in kzsTCPSocketDelete");
    }
    tcpSocket->server = TCP_SOCKET_ERROR;

    tcpSocket->connected = KZ_TRUE;
    kzsSuccess();
}

static kzsError kzsTCPSocketClientConnectThread(void* data)
{
    kzsError result;
    struct KzsTCPSocket* tcpSocket = (struct KzsTCPSocket*)data;
    /* Connect to server. */
    result = connect(tcpSocket->client, (struct sockaddr*)&tcpSocket->socketAddress, sizeof(tcpSocket->socketAddress));
    if(result == TCP_SOCKET_ERROR)
    {
        #if KZ_TCP_SOCKET_DEBUG
        printf("kzsTCPSocketClientConnectThread(): Sockets operation connect() FAILED\n");
        #endif
        kzsErrorThrow(KZS_ERROR_SOCKET_OPERATION_FAILED, "Error in kzsTCPSocketClientConnectThread");
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

    tcpSocket = kzsMalloc(sizeof(*tcpSocket));
    kzsErrorTest(tcpSocket != KZ_NULL, KZS_ERROR_OUT_OF_MEMORY, "Out of memory while creating tcp socket");
    tcpSocket->socketHandle = TCP_SOCKET_ERROR;
    tcpSocket->client = TCP_SOCKET_ERROR;
    tcpSocket->server = TCP_SOCKET_ERROR;
    tcpSocket->port = port;
    tcpSocket->connectThread = KZ_NULL;
    tcpSocket->connected = KZ_FALSE;

    tcpSocket->socketAddress.sin_family = AF_INET;
    if(kzsStrcmp(inetAddress, "") == 0)
    {
        tcpSocket->socketAddress.sin_addr.s_addr = INADDR_ANY;
    }
    else
    {
        tcpSocket->socketAddress.sin_addr.s_addr = inet_addr(inetAddress);
    }
    tcpSocket->socketAddress.sin_port = tcpSocket->port;
    tcpSocket->server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(tcpSocket->server == TCP_SOCKET_ERROR) 
    {
        #if KZ_TCP_SOCKET_DEBUG
        printf("kzsTCPSocketCreate(): Sockets operation socket() FAILED\n");
        #endif
        *out_tcpSocket = tcpSocket;
        kzsErrorForward(KZS_ERROR_SOCKET_OPERATION_FAILED);
    }

    result = bind(tcpSocket->server, (struct sockaddr*) &tcpSocket->socketAddress, sizeof(tcpSocket->socketAddress));
    if(result == TCP_SOCKET_ERROR)
    {
        #if KZ_TCP_SOCKET_DEBUG
        printf("kzsTCPSocketCreate(): Sockets operation bind() FAILED\n");
        #endif
        *out_tcpSocket = tcpSocket;
        kzsErrorForward(KZS_ERROR_SOCKET_OPERATION_FAILED);
    }

    result = listen(tcpSocket->server, KZ_TCP_SOCKET_CONNECTION_ATTEMPT_COUNT);
    if(result == TCP_SOCKET_ERROR)
    {
        #if KZ_TCP_SOCKET_DEBUG
        printf("kzsTCPSocketCreate(): Sockets operation listen() FAILED\n");
        #endif
        *out_tcpSocket = tcpSocket;
        kzsErrorForward(KZS_ERROR_SOCKET_OPERATION_FAILED);
    }

    #if KZ_TCP_SOCKET_DEBUG
    printf("kzsTCPSocketCreate(): Listening on socket...\n");
    #endif

    tcpSocket->connected = KZ_FALSE;
    result = kzsThreadCreate(kzsTCPSocketServerConnectThread, tcpSocket, KZ_TRUE, &tcpSocket->connectThread); 
    kzsErrorForward(result);

    *out_tcpSocket = tcpSocket;
    kzsSuccess();
}

kzsError kzsTCPSocketCreateClient(kzString inetAddress, kzUint port, struct KzsTCPSocket** out_tcpSocket)
{
    kzsError result;
    struct KzsTCPSocket* tcpSocket;

    tcpSocket = kzsMalloc(sizeof(*tcpSocket));
    kzsErrorTest(tcpSocket != KZ_NULL, KZS_ERROR_OUT_OF_MEMORY, "Out of memory while creating tcp socket");
    tcpSocket->socketHandle = TCP_SOCKET_ERROR;
    tcpSocket->client = TCP_SOCKET_ERROR;
    tcpSocket->server = TCP_SOCKET_ERROR;
    tcpSocket->port = port;
    tcpSocket->connectThread = KZ_NULL;
    tcpSocket->connected = KZ_FALSE;
    tcpSocket->socketAddress.sin_family = AF_INET;
    if(kzsStrcmp(inetAddress, "") == 0)
    {
        tcpSocket->socketAddress.sin_addr.s_addr = INADDR_ANY;
    }
    else
    {
        tcpSocket->socketAddress.sin_addr.s_addr = inet_addr(inetAddress);
    }
    tcpSocket->socketAddress.sin_port = tcpSocket->port;
    tcpSocket->client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(tcpSocket->client == TCP_SOCKET_ERROR) 
    {
        #if KZ_TCP_SOCKET_DEBUG
        printf("kzsTCPSocketCreateClient(): Windows Sockets operation socket() FAILED\n");
        #endif
        *out_tcpSocket = tcpSocket;
        kzsErrorThrow(KZS_ERROR_SOCKET_OPERATION_FAILED, "Error in kzsTCPSocketCreateClient");
    }

    tcpSocket->connected = KZ_FALSE;
    result = kzsThreadCreate(kzsTCPSocketClientConnectThread, tcpSocket, KZ_TRUE, &tcpSocket->connectThread); 
    kzsErrorForward(result);

    *out_tcpSocket = tcpSocket;
    kzsSuccess();
}

kzsError kzsTCPSocketDelete(struct KzsTCPSocket* tcpSocket)
{
    kzsError result;
    if(tcpSocket->connected)
    {
        if(tcpSocket->client != TCP_SOCKET_ERROR)
        {
            result = shutdown(tcpSocket->client, 2);
            if(result == TCP_SOCKET_ERROR  
                )
            {
                #if KZ_TCP_SOCKET_DEBUG
                printf("kzsTCPSocketDelete(): Sockets operation shutdown() FAILED\n");
                #endif
                kzsErrorThrow(KZS_ERROR_SOCKET_OPERATION_FAILED, "Error in kzsTCPSocketDelete");
            }

            result = close(tcpSocket->client);
            if(result == TCP_SOCKET_ERROR)
            {
                #if KZ_TCP_SOCKET_DEBUG
                printf("kzsTCPSocketDelete(): Sockets operation close() FAILED: CLIENT\n");
                #endif
                kzsErrorThrow(KZS_ERROR_SOCKET_OPERATION_FAILED, "Error in kzsTCPSocketDelete");
            }
            tcpSocket->client = TCP_SOCKET_ERROR;

            #if KZ_TCP_SOCKET_DEBUG
            printf("kzsTCPSocketDelete(): CLIENT DONE\n");
            #endif
        }
        else
        {
            result = shutdown(tcpSocket->socketHandle, 2);
            if(result == TCP_SOCKET_ERROR)
            {
                #if KZ_TCP_SOCKET_DEBUG
                printf("kzsTCPSocketDelete(): Sockets operation shutdown() FAILED\n");
                #endif
                kzsErrorThrow(KZS_ERROR_SOCKET_OPERATION_FAILED, "Error in kzsTCPSocketDelete");
            }

            result = close(tcpSocket->socketHandle);
            if(result == TCP_SOCKET_ERROR)
            {
                #if KZ_TCP_SOCKET_DEBUG
                printf("kzsTCPSocketDelete(): Sockets operation close() FAILED: SERVER\n");
                #endif
                kzsErrorThrow(KZS_ERROR_SOCKET_OPERATION_FAILED, "Error in kzsTCPSocketDelete");
            }
            tcpSocket->socketHandle = TCP_SOCKET_ERROR;

            #if KZ_TCP_SOCKET_DEBUG
            printf("kzsTCPSocketDelete(): SERVER DONE\n");
            #endif
        }
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
    if(tcpSocket->client != TCP_SOCKET_ERROR)
    {
        bytesRead = recv((tcpSocket->client), (kzChar*)out_bytes, byteCount, 0);
    }
    else
    {
        bytesRead = recv((tcpSocket->socketHandle), (kzChar*)out_bytes, byteCount, 0);
    }
    if(bytesRead == TCP_SOCKET_ERROR)
    {
        #if KZ_TCP_SOCKET_DEBUG
        printf("kzsTCPSocketReadBytes(): Sockets operation recv() FAILED\n");
        #endif
        /* Could not read any more bytes, but don't treat this situation as an error. */
        bytesRead = 0;
    } 
    return bytesRead;
}

kzUint kzsTCPSocketWriteBytes(const struct KzsTCPSocket* tcpSocket, kzUint byteCount, const kzByte* bytes)
{
    kzInt bytesWritten;
    if(tcpSocket->client != TCP_SOCKET_ERROR)
    {
        bytesWritten = send((tcpSocket->client), (kzChar*)bytes, byteCount, 0);
    }
    else
    {
        bytesWritten = send((tcpSocket->socketHandle), (kzChar*)bytes, byteCount, 0);
    }

    if(bytesWritten == TCP_SOCKET_ERROR)
    {
        #if KZ_TCP_SOCKET_DEBUG
        printf("kzsTCPSocketWriteBytes(): Sockets operation send() FAILED\n");
        #endif
        bytesWritten = 0;
    }
    return bytesWritten;
}

kzBool kzsTCPSocketIsConnected(const struct KzsTCPSocket* tcpSocket)
{
    return tcpSocket->connected;
}
