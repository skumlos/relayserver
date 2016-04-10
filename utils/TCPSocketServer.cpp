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


#include "TCPSocketServer.h"
#include "TCPSocketServerCallback.h"
#include "Socket.h"
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <errno.h>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <sstream>
#define BACKLOG 10     // how many pending connections queue will hold

template <class T>
std::string toString(T t) {
	std::stringstream str;
	str << t;
	return str.str();
};

void sigchld_handler(int s)
{
    while(waitpid(-1, NULL, WNOHANG) > 0);
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

TCPSocketServer::TCPSocketServer(int port, TCPSocketServerCallback* callback) :
	m_sockfd(0),
	m_port(port),
	m_callback(callback)
{
}

TCPSocketServer::~TCPSocketServer()
{
	stop();
	if(m_sockfd != 0) {
		close(m_sockfd);
	}
}

void TCPSocketServer::thread()
{
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage their_addr;
	socklen_t sin_size;
	struct sigaction sa;
	int yes=1;
	char s[INET6_ADDRSTRLEN];
	int rv;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use local IP

	if ((rv = getaddrinfo(NULL, toString(m_port).c_str(), &hints, &servinfo)) != 0) {
		std::fprintf(stderr, "TCPSocketServer: getaddrinfo: %s\n", gai_strerror(rv));
		return; // TODO: FIX
	}

	// bind to the first possible
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((m_sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			std::perror("TCPSocketServer: socket");
			continue;
		}

		if (setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(int)) == -1) {
			std::perror("TCPSocketServer: setsockopt");
			return;
		}

		if (bind(m_sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(m_sockfd);
			m_sockfd = 0;
			std::perror("TCPSocketServer: bind");
			continue;
		}
		break;
	}

	if (p == NULL)  {
		std::fprintf(stderr, "TCPSocketServer: failed to bind\n");
		return; // TODO: FIX
	}

	freeaddrinfo(servinfo);

	if (listen(m_sockfd, BACKLOG) == -1) {
		std::perror("listen");
		return;
	}

	sa.sa_handler = sigchld_handler; // reap dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		std::perror("sigaction");
		return;
	}

	std::printf("TCPSocketServer: Waiting for connections...\n");

	while(1) {  // accept() loop
		int* new_fd = new int;
		sin_size = sizeof their_addr;
		(*new_fd) = accept(m_sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if (*new_fd == -1) {
			std::perror("accept");
			continue;
		}

		inet_ntop(their_addr.ss_family,
		get_in_addr((struct sockaddr *)&their_addr),s, sizeof s);
//		std::printf("TCPSocketServer (%d): got connection from %s\n", m_port, s);
		std::string host(s);
		TCPSocket* newSocket = new TCPSocket(new_fd,host,m_port);
		if(m_callback != NULL) {
			m_callback->socketConnected(newSocket,this);
		}
		clientConnected(newSocket);
	}
}

