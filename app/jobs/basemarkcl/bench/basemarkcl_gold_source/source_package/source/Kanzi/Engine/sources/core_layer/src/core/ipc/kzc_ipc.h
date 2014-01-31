/**
* \file 
* Inter-process communication common interface.
* 
* Copyright 2009-2011 by Rightware. All rights reserved.
*/
#ifndef KZC_IPC_H
#define KZC_IPC_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations. */
struct KzcMemoryManager;


/**
 * \struct KzcIpc
 * Structure for inter-process communication.
 */
struct KzcIpc;


/* TODO: Rename to KzcIpcCustom... */
/* TODO: Document */
/* TODO: Make these to return kzsError */
typedef kzsError (*KzcCustomIpcDelete)(void* customImplementation); /* TODO: Add postfix to the name indicating it's a callback (IpcFunction) */
typedef kzUint (*KzcCustomIpcReadBytes)(void* customImplementation, kzUint byteCount, kzByte* out_bytes); /* TODO: Rename customImplementation to userData */
typedef kzUint (*KzcCustomIpcWriteBytes)(void* customImplementation, kzUint byteCount, const kzByte* bytes);
typedef kzBool (*KzcCustomIpcIsConnected)(const void* customImplementation);


/** Creates new interprocess communication structure using TCP sockets on local host. */ /* TODO: tcpPort as U16 */
/* TODO: Add optional callback for notification when the TPC is connected. */
kzsError kzcIpcCreateTcp(const struct KzcMemoryManager* memoryManager, kzUint tcpPort, kzBool isServerSocket, struct KzcIpc** out_ipc);

/** Creates new interprocess communication structure using custom implementation. */
kzsError kzcIpcCreateCustom(const struct KzcMemoryManager* memoryManager, void* customImplementation, KzcCustomIpcDelete deleteFunction,
                            KzcCustomIpcReadBytes readFunction, KzcCustomIpcWriteBytes writeFunction, KzcCustomIpcIsConnected isConnectedFunction, 
                            struct KzcIpc** out_ipc);

/** Deletes and disables inter-process communication. */
kzsError kzcIpcDelete(struct KzcIpc* ipc);

/** Reads bytes from another process. TODO: Document what it returns unless output parameter name explains. */
kzUint kzcIpcReadBytes(struct KzcIpc* ipc, kzUint byteCount, kzByte* out_bytes);

/** Writes bytes for another process. */
kzUint kzcIpcWriteBytes(struct KzcIpc* ipc, kzUint byteCount, const kzByte* bytes);

/** Returns if ipc is connected. */
kzBool kzcIpcIsConnected(const struct KzcIpc* ipc);


#endif
