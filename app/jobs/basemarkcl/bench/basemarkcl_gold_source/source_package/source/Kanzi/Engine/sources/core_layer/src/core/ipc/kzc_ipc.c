/**
* \file 
* Inter-process communication common interface implementation.
* 
* Copyright 2009-2011 by Rightware. All rights reserved.
*/
/* TODO: Include own header file first. */

#include <system/socket/kzs_tcp_socket.h>
#include <core/ipc/kzc_ipc.h>
#include <core/debug/kzc_log.h>
#include <core/memory/kzc_memory_manager.h>


/* TODO: KZC_IPC... */
#define KZ_IPC_LOCALHOST "127.0.0.1"


/** Structure for custom ipc implementation. */
struct KzcCustomIpc
{
    void* customImplementation; /* TODO: Comment */
    KzcCustomIpcDelete deleteFunction;
    KzcCustomIpcReadBytes readFunction;
    KzcCustomIpcWriteBytes writeFunction;
    KzcCustomIpcIsConnected isConnectedFunction;
};

struct KzcIpc
{
    enum KzcInputStreamType
    {
        KZC_IPC_TCP_SOCKET, /* TODO: Comment */
        KZC_IPC_TYPE_CUSTOM
    } type; /**< Type of the ipc implementation. */

    union
    {
        struct KzsTCPSocket* tcpSocket;
        struct KzcCustomIpc customIpc;
    } ipcImplementation; /**< Target of the ipc implementation. */
};


kzsError kzcIpcCreateTcp(const struct KzcMemoryManager* memoryManager, kzUint tcpPort, kzBool isServerSocket, struct KzcIpc** out_ipc)
{
    kzsError result;
    struct KzcIpc* ipc;

    result = kzcMemoryAllocVariable(memoryManager, ipc, "InterProcessCommunication");
    kzsErrorForward(result);

    if(isServerSocket)
    {
        result = kzsTCPSocketCreateServer(KZ_IPC_LOCALHOST, tcpPort, &ipc->ipcImplementation.tcpSocket);
    }
    else
    {
        result = kzsTCPSocketCreateClient(KZ_IPC_LOCALHOST, tcpPort, &ipc->ipcImplementation.tcpSocket);
    }
    kzsErrorForward(result); /* TODO: Move up */
    ipc->type = KZC_IPC_TCP_SOCKET;

    *out_ipc = ipc;
    kzsSuccess();
}

kzsError kzcIpcCreateCustom(const struct KzcMemoryManager* memoryManager, void* customImplementation, KzcCustomIpcDelete deleteFunction,
                            KzcCustomIpcReadBytes readFunction, KzcCustomIpcWriteBytes writeFunction, KzcCustomIpcIsConnected isConnectedFunction, 
                            struct KzcIpc** out_ipc)
{
    kzsError result;
    struct KzcIpc* ipc;
    struct KzcCustomIpc customIpc;

    result = kzcMemoryAllocVariable(memoryManager, ipc, "InterProcessCommunication");
    kzsErrorForward(result);

    customIpc.customImplementation = customImplementation;
    customIpc.deleteFunction = deleteFunction;
    customIpc.readFunction = readFunction;
    customIpc.writeFunction = writeFunction;
    customIpc.isConnectedFunction = isConnectedFunction;

    ipc->type = KZC_IPC_TYPE_CUSTOM;
    ipc->ipcImplementation.customIpc = customIpc;

    *out_ipc = ipc;
    kzsSuccess();
}

kzsError kzcIpcDelete(struct KzcIpc* ipc)
{
    kzsError result;
    switch (ipc->type)
    {
        case KZC_IPC_TCP_SOCKET:
        {
            result = kzsTCPSocketDelete(ipc->ipcImplementation.tcpSocket);
            kzsErrorForward(result);
            break;
        }
        case KZC_IPC_TYPE_CUSTOM:
        {
            struct KzcCustomIpc* customIpc = &ipc->ipcImplementation.customIpc;
            result = ipc->ipcImplementation.customIpc.deleteFunction(customIpc->customImplementation);
            kzsErrorForward(result);
            break;
        }
        default:
        {
            kzsErrorThrow(KZS_ERROR_ENUM_OUT_OF_RANGE, "Invalid IPC type");
        }
    }
    result = kzcMemoryFreeVariable(ipc);
    kzsErrorForward(result);

    kzsSuccess();
}

kzUint kzcIpcReadBytes(struct KzcIpc* ipc, kzUint byteCount, kzByte* out_bytes)
{
    kzUint bytesRead = 0;
    switch (ipc->type)
    {
        case KZC_IPC_TCP_SOCKET:
        {
            bytesRead = kzsTCPSocketReadBytes(ipc->ipcImplementation.tcpSocket, byteCount, out_bytes);
            break;
        }
        case KZC_IPC_TYPE_CUSTOM:
        {
            struct KzcCustomIpc* customIpc = &ipc->ipcImplementation.customIpc;
            bytesRead = ipc->ipcImplementation.customIpc.readFunction(customIpc->customImplementation, byteCount, out_bytes);
            break;
        }
        default:
        {
            kzsLog(KZS_LOG_LEVEL_WARNING, "Invalid IPC type");
        }
    }
    return bytesRead;
}

kzUint kzcIpcWriteBytes(struct KzcIpc* ipc, kzUint byteCount, const kzByte* bytes)
{
    kzUint bytesWritten = 0;
    switch (ipc->type)
    {
        case KZC_IPC_TCP_SOCKET:
        {
            bytesWritten = kzsTCPSocketWriteBytes(ipc->ipcImplementation.tcpSocket, byteCount, bytes);
            break;
        }
        case KZC_IPC_TYPE_CUSTOM:
        {
            struct KzcCustomIpc* customIpc = &ipc->ipcImplementation.customIpc;
            bytesWritten = ipc->ipcImplementation.customIpc.writeFunction(customIpc->customImplementation, byteCount, bytes);
            break;
        }
        default:
        {
            kzsLog(KZS_LOG_LEVEL_WARNING, "Invalid IPC type");
        }
    }
    return bytesWritten;
}

kzBool kzcIpcIsConnected(const struct KzcIpc* ipc)
{
    kzBool success = KZ_TRUE;
    switch (ipc->type)
    {
        case KZC_IPC_TCP_SOCKET:
        {
            success = kzsTCPSocketIsConnected(ipc->ipcImplementation.tcpSocket);
            break;
        }
        case KZC_IPC_TYPE_CUSTOM:
        {
            const struct KzcCustomIpc* customIpc = &ipc->ipcImplementation.customIpc;
            success = ipc->ipcImplementation.customIpc.isConnectedFunction(customIpc->customImplementation);
            break;
        }
        default:
        {
            kzsLog(KZS_LOG_LEVEL_WARNING, "Invalid IPC type");
        }
    }
    return success;
}
