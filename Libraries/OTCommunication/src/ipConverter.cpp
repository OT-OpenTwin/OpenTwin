// @otlicense
// File: ipConverter.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

// OpenTwin header
#include "OTCommunication/IpConverter.h"		// Corresponding header

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

std::string ot::IpConverter::hostFromIp(
	const std::string& _ip
) {
	return _ip.substr(0, _ip.rfind(':'));
}


