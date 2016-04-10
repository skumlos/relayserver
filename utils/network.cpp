#include "network.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <iomanip>

std::string network::getAddress(const std::string& iface, const AddressTypes type) {
        int fd;
        struct ifreq ifr;

        fd = socket(AF_INET, SOCK_DGRAM, 0);
        ifr.ifr_addr.sa_family = AF_INET;

        strncpy(ifr.ifr_name, iface.c_str(), IFNAMSIZ-1);
	int r = 0;
	std::string ret = "";
	switch(type) {
		case AT_MAC:
			r = ioctl(fd, SIOCGIFHWADDR, &ifr);
			if(r == 0) {
				size_t L = 18;
				char mac[L];
				snprintf(mac,L,"%.02X:%.02X:%.02X:%.02X:%.02X:%.02X", (unsigned char)(ifr.ifr_hwaddr.sa_data[0]),
										(unsigned char)(ifr.ifr_hwaddr.sa_data[1]),
										(unsigned char)(ifr.ifr_hwaddr.sa_data[2]),
										(unsigned char)(ifr.ifr_hwaddr.sa_data[3]),
										(unsigned char)(ifr.ifr_hwaddr.sa_data[4]),
										(unsigned char)(ifr.ifr_hwaddr.sa_data[5]));
				ret = mac;
			}
		break;
		default:
		case AT_IPV4:
			r = ioctl(fd, SIOCGIFADDR, &ifr);
			if(r == 0) {
				ret = inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
			}
		break;
	}

        close(fd);
	return ret;
}
