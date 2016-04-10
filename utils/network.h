#ifndef NETWORK_H
#define NETWORK_H

#include <string>

namespace network {
	enum AddressTypes {
		AT_IPV4,
		AT_MAC
	};

	std::string getAddress(const std::string& iface, const AddressTypes type);
};

#endif /* NETWORK_H */
