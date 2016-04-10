/**
 * Copyright (c) 2013, Martin Hejnfelt (martin@hejnfelt.com)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


/**
 * This is a "generic" TCP socket class, which
 * basically is a wrapper around a C socket,
 * to make it encapsulated and more C++ like...
 *
 * 2012, Martin Hejnfelt (martin@hejnfelt.com)
 */

#ifndef TCPSOCKET_H
#define TCPSOCKET_H
#include "Socket.h"

class TCPSocket : public Socket {
	public:
		TCPSocket(std::string hostname, int port);
		TCPSocket(int* sockfd, std::string hostname, int port);
		virtual ~TCPSocket();
		int getPort() { return m_port; };
		std::string getHostname() { return m_hostname; };
		void send(const std::string& message);
		void send(unsigned char* toSend, int numBytes);
		int recv(std::string& buffer);
		int recv(std::string& buffer, int recvBytes);
		int recv(std::string& buffer, const std::string& termination);
		int peek();
		int poll(int timeout_ms = 0);
		bool isOpen();
	private:
		std::string m_hostname;
		int m_port;
		void* get_in_addr(struct sockaddr *sa);
		int *m_sockfd;
};

#endif /* TCPSOCKET_H */
