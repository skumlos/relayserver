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


#include "Uart.h"
#include <unistd.h>
#include <fcntl.h>
#include <cstdio>
#include <errno.h>
#include <sys/poll.h>
#include <iostream>

UART::UART(std::string tty, bool hwflowctrl) throw (UARTException) :
	m_fd(0),
	m_tty(tty),
	m_hwflowctrl(hwflowctrl)
{
	m_fd = open(tty.c_str(), O_RDWR|O_NOCTTY);
	if(m_fd < 0) {
		throw UARTException("Could not open file.");
	}
	try {
		set_8N1();
	} catch (UARTException& ex) {
		throw ex;
	}
	pthread_mutex_init(&m_portmutex,0);
	printf("Opened %s\n",tty.c_str());
}

UART::~UART()
{
	pthread_mutex_destroy(&m_portmutex);
}

void UART::flush()
{
	tcflush(m_fd,TCIOFLUSH);
}

int UART::poll(int timeout_ms)
{
	struct pollfd pfd = {m_fd, POLLIN, 0};
	int err;

        err = ::poll(&pfd, 1, timeout_ms);
        if (err == -1) {
                perror("UART: poll failed");
                return -1;
        }
	return err;
}

void UART::setSpeed(speed_t new_speed) throw (UARTException)
{
	struct termios tnew;
        int err;

	tcgetattr(m_fd, &tnew);

        /* speed settings */
        cfsetispeed(&tnew, new_speed);
        cfsetospeed(&tnew, new_speed);

	err =  tcsetattr(m_fd, TCSAFLUSH, &tnew);
        if (err) {
                throw UARTException("Could not set speed");
        }
	return;
}

void UART::set_8N1() throw (UARTException)
{
	struct termios tnew;
        int err;

        err = tcgetattr(m_fd, &tnew);
        if (err) {
		printf("C %d\n",errno);
		throw UARTException("Could not get port attributes");
        }

        tnew.c_cflag |= (CLOCAL | CREAD);

        tnew.c_cflag &= ~PARENB;
        tnew.c_cflag &= ~CSTOPB;
        tnew.c_cflag &= ~CSIZE;
        tnew.c_cflag |=  CS8;

	if(m_hwflowctrl) {
		tnew.c_cflag |= CRTSCTS;
	} else {
		tnew.c_cflag &= ~CRTSCTS;
	}

        tnew.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
        tnew.c_iflag &= ~(IXON | IXOFF | IXANY);
        tnew.c_iflag &= ~(ICRNL | INLCR);
        tnew.c_oflag &= ~OPOST;
        tnew.c_cc[VTIME] = 0;
        tnew.c_cc[VMIN]  = 1;

        err = tcsetattr(m_fd, TCSAFLUSH, &tnew);
        if (err) {
		throw UARTException("Could not set port attributes");
        }
	return;
}

void UART::lock() { pthread_mutex_lock(&m_portmutex); }

void UART::unlock() { pthread_mutex_unlock(&m_portmutex); }

unsigned char UART::readByte() throw (UARTException) {
	unsigned char c = 0;
	if(::read(m_fd,&c,1) < 0) {
		throw UARTException("readByte: No data available.");
	}
	return c;
}

int UART::read(std::string& buf) throw (UARTException)
{
	char _buf;
	while(::read(m_fd,&_buf,1) > 0) {
		buf += _buf;
	}
	return 0;
}

void UART::write(const std::string& msg) throw (UARTException)
{
	flush();
	int written = ::write(m_fd,msg.c_str(),msg.size());
	if(written != (int)msg.size()) {
		throw UARTException("Problem sending all data");
	}
	return;
}

void UART::write(const std::vector<unsigned char>& msg) throw (UARTException)
{
	flush();
	int written = 0;
	for(unsigned int j = 0; j < msg.size(); j++) {
		written += ::write(m_fd,&msg[j],1);
	}
	if(written != (int)msg.size()) {
		throw UARTException("Problem sending all data");
	}
	return;
}
