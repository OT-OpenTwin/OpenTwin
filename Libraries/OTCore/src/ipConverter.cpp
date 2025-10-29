// @otlicense

 // SessionService header
#include <openTwin/ipConverter.h>		// Corresponding header

std::string ot::ipConverter::filterIpFromSender(
	const std::string &				_senderIP,
	const std::string &				_senderPort
) {
	std::string senderIP = _senderIP;

	if (senderIP.substr(0, 5) == "Some(") senderIP = senderIP.substr(5);
	if (senderIP.substr(0, 3) == "V4(") senderIP = senderIP.substr(3);

	// We have an IP V4 address
	size_t index = senderIP.rfind(':');
	if (index == -1) throw std::exception();
	senderIP = senderIP.substr(0, index + 1);
	return senderIP.append(_senderPort);
}

std::string ot::ipConverter::portFromIp(
	const std::string &				_ip
) {
	return _ip.substr(_ip.rfind(':') + 1);
}

