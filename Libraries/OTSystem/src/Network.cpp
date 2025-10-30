// @otlicense
// File: Network.cpp
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

#include "OTSystem/Network.h"
#include "OTSystem/OperatingSystem.h"

#include <iostream>

#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#endif

#ifdef _DEBUG
#define DEBUGMSG(___message, ...) printf(___message, ##__VA_ARGS__)
#define DEBUGMSGW(___message, ...) wprintf(___message, ##__VA_ARGS__)
#else
#define DEBUGMSG(___message, ...)
#define DEBUGMSGW(___message, ...)
#endif // _DEBUG

static int c_lastErrorCode;

#if defined(_WIN32) || defined(_WIN64)
std::wstring translateErrorCode(unsigned long _errorCode)
{
	LPTSTR s;
	if (FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, _errorCode, 0, (LPTSTR)&s, 0, NULL) == 0) {
		return L"Error code: " + std::to_wstring(_errorCode);
	}
	else
	{
		std::wstring ret(s);
		delete s;
		return ret;
	}
}
#endif // 

//#include <boost/asio.hpp>
//
//bool isPortInUse(ot::net::port_t _port) {
//	using namespace boost::asio;
//	using ip::tcp;
//
//	io_service svc;
//	tcp::acceptor a(svc);
//
//	boost::system::error_code ec;
//	a.open(tcp::v4(), ec) || a.bind({ tcp::v4(), port }, ec);
//
//	return ec == error::address_in_use;
//}

#ifdef OS_WINDOWS
unsigned long ot::net::checkTCPConnection(port_t _port, const char * _ip) {
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) {
		c_lastErrorCode = iResult;
		return c_lastErrorCode;
	}

	struct sockaddr_in client;
	SOCKET sock;
	client.sin_family = AF_INET;
	client.sin_port = htons(_port);
	client.sin_addr.s_addr = inet_addr(_ip);
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) {
		c_lastErrorCode = WSAGetLastError();
		WSACleanup();
		return c_lastErrorCode;
	}

	int result = connect(sock, (struct sockaddr *) &client, sizeof(client));
	if (result == SOCKET_ERROR) {
		c_lastErrorCode = WSAGetLastError();
		WSACleanup();
		return c_lastErrorCode;
	}
	else
	{
		WSACleanup();
		closesocket(sock);
		return 0;
	}
}
#endif // OS_WINDOWS

std::wstring ot::net::getLastError(void) {
	return translateErrorCode(c_lastErrorCode);
}