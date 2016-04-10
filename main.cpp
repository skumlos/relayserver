#include <wiringPi.h>
#include <cstdio>
#include "utils/TCPSocketServer.h"
#include "utils/Thread.h"
#include <unistd.h>
#include <csignal>
#include "utils/TCPSocket.h"
#include <sstream>
#include "3rdparty/cajun-2.0.2/json/reader.h"
#include "3rdparty/cajun-2.0.2/json/writer.h"

static const int relays[] = {11, 9, 10, 22, 21, 17, 4, 1};

class Worker : public Thread
{
public:
	Worker(TCPSocket* socket) : Thread(true),m_socket(socket) {
		printf("Worker: Started for %s\n",m_socket->getHostname().c_str());
		start();
	};

	virtual ~Worker() {
		delete m_socket;
	};

	void thread() {
		signal(SIGPIPE,SIG_IGN);
		try {
			while(1) {
				bool data_available = false;
				while(!data_available) {
					data_available = ((m_socket->poll(1000) == 0) ? true : false);
					if(data_available && m_socket->peek() == 0) {
						printf("Worker: Socket seems closed, bailing...\n");
						throw false;
				       	}
				}
				std::string buffer = "";
				m_socket->recv(buffer,4);
				unsigned int toReceive = 0;
				toReceive += ((unsigned char)buffer[0] << 24);
				toReceive += ((unsigned char)buffer[1] << 16);
				toReceive += ((unsigned char)buffer[2] << 8);
				toReceive += ((unsigned char)buffer[3] << 0);
				m_socket->recv(buffer,toReceive);

				std::istringstream ist(buffer);
				json::Object request;
				json::Reader::Read(request,ist);
				printf("Worker: Request received\n");
				printf("%s\n",ist.str().c_str());

				json::Object response;
				std::ostringstream ost;
				std::string req = json::String(request["type"]).Value();
				int relayNo = json::Number(request["output"]).Value();
				response["type"] = json::String(req);
				try {
					if(relayNo > 8 || relayNo < 1) {
						throw false;
					}
					relayNo -= 1; // to accomodate for the zero index
					if(req == "activate") {
						printf("Activate relay %d (%d)\n",relayNo,relays[relayNo]);
						digitalWrite(relays[relayNo],LOW);
						response["result"] = json::Boolean(true);
					} else if(req == "deactivate") {
						printf("Deactivate relay %d (%d)\n",relayNo,relays[relayNo]);
						digitalWrite(relays[relayNo],HIGH);
						response["result"] = json::Boolean(true);
					} else if(req == "getstate") {
						int state = digitalRead(relays[relayNo]);
						printf("Relay %d (%d) has state %d\n",relayNo,relays[relayNo],state);
						response["state"] = json::Number(state);
					}
				} catch (bool ex) {
					response["result"] = json::Boolean(false);
				}
				json::Writer::Write(response,ost);
				unsigned char sendHeader[4];
				size_t len = ost.str().size();
				sendHeader[0] = (unsigned char) (len >> 24) & 0xFF;
				sendHeader[1] = (unsigned char) (len >> 16) & 0xFF;
				sendHeader[2] = (unsigned char) (len >> 8) & 0xFF;
				sendHeader[3] = (unsigned char) (len >> 0) & 0xFF;

				m_socket->send(sendHeader,4);
				m_socket->send(ost.str());
			}
		} catch (std::exception& ex) {
			printf("Worker: Exception detected (%s)\n",ex.what());
		} catch (bool ex) {
			// Exception in socket, probably closed
			printf("Worker: Exception detected\n");
		}
	};
private:
	TCPSocket *m_socket;
};

class Server : public TCPSocketServer {
public:
	Server(int port) : TCPSocketServer(port) {
		start();
	};
protected:
	virtual void clientConnected(TCPSocket* socket) {
		new Worker(socket);
	};
};

int main(int argc, char* argv[]) {
	if(wiringPiSetupGpio() < 0) {
        	fprintf(stderr, "Could not setup wiringPi\n");
		return 1;
	}
	Server* server = new Server(4567);
	while(server->isRunning()) { sleep(2); };
	return 0;
}
