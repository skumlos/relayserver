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
 * C++ wrapper for UART communications
 *
 * 2011, Martin Hejnfelt (martin@hejnfelt.com)
 */

#ifndef UART_H
#define UART_H
#include <stdint.h>
#include <termios.h>
#include <string>
#include <vector>
#include <pthread.h>
#include <stdexcept>

class UARTException : public std::runtime_error {
public:
	UARTException(std::string reason) : std::runtime_error(reason) {};
};

class UART {
	public:
		UART(std::string tty, bool hwflowctrl = false) throw (UARTException);
		virtual ~UART();
		void lock();
		void unlock();
		void setSpeed(speed_t new_speed) throw (UARTException);
		void write(const std::string& msg) throw (UARTException);
		void write(const std::vector<unsigned char>& msg) throw (UARTException);
		int read(std::string& buf) throw (UARTException);
		unsigned char readByte() throw (UARTException);
		void flush();
		int poll(int timeout_ms = -1);
	private:
		void set_8N1() throw (UARTException);
		int m_fd;
		std::string m_tty;
		bool m_hwflowctrl;
		pthread_mutex_t m_portmutex;
};

#endif /* UART_H */
