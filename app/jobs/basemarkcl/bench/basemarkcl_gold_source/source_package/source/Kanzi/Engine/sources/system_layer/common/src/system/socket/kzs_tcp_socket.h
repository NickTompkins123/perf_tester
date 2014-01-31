/**
* \file 
* Socket for reading and writing bytes via TCP.
* 
* Copyright 2009-2011 by Rightware. All rights reserved.
*/
#ifndef KZS_TCP_SOCKET_H
#define KZS_TCP_SOCKET_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>

/**
 * \struct KzsTCPSocket
 * TCP socket.
 */
struct KzsTCPSocket;


/** Creates a new TCP server socket connection using specified address and port. */
kzsError kzsTCPSocketCreateServer(kzString inetAddress, kzUint port, struct KzsTCPSocket** out_tcpSocket);

/** Creates a new TCP client socket connection using specified address and port. */
kzsError kzsTCPSocketCreateClient(kzString inetAddress, kzUint port, struct KzsTCPSocket** out_tcpSocket);

/** Deletes and does clean-up for tcp socket. */
kzsError kzsTCPSocketDelete(struct KzsTCPSocket* tcpSocket);

/** Reads bytes from TCP Socket. */
kzUint kzsTCPSocketReadBytes(const struct KzsTCPSocket* tcpSocket, kzUint byteCount, kzByte* out_bytes);

/** Writes bytes for TCP Socket. */
kzUint kzsTCPSocketWriteBytes(const struct KzsTCPSocket* tcpSocket, kzUint byteCount, const kzByte* bytes);

/** Returns if TCP socket is connected. */
kzBool kzsTCPSocketIsConnected(const struct KzsTCPSocket* tcpSocket);


#endif
