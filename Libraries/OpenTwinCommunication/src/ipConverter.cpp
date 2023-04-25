/*
 * IpConverter.cpp
 *
 *  Created on: November 30, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2020 openTwin
 */

 // OpenTwin header
#include "OpenTwinCommunication/IpConverter.h"		// Corresponding header

std::string ot::IpConverter::filterIpFromSender(
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

std::string ot::IpConverter::portFromIp(
	const std::string &				_ip
) {
	return _ip.substr(_ip.rfind(':') + 1);
}

