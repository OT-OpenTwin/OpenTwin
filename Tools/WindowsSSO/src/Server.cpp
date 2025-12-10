// @otlicense
// File: Server.cpp
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

#include "Server.h"
#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include "Base64Encoding.h"

using boost::asio::ip::tcp;
Server::Server(const std::string& _ip, int _port, AuthorisationServiceAPI& _authorisationService)
{
    boost::asio::io_context io;
    tcp::endpoint endpoint(
        boost::asio::ip::make_address(_ip), 
        _port                                        
    );
    tcp::acceptor acceptor(io, endpoint);
    std::cout << "Server listening on " + _ip + ":" + std::to_string(_port) + "...\n";

    tcp::socket socket(io);
    acceptor.accept(socket);
    std::cout << "Client connected.\n";
    
    boost::asio::streambuf buf;
    boost::asio::read_until(socket, buf, '\n');
    std::string msg (boost::asio::buffer_cast<const char*>(buf.data()),buf.size());
	buf.consume(buf.size());

	std::string token1Encoded = msg.substr(0, msg.length() - 1); // remove newline

	std::cout << "Received token1: " << token1Encoded << "\n";
	std::cout << "Token size: " << token1Encoded.size() << "\n";
	std::vector<unsigned char> token1 = decode(token1Encoded);
    std::cin.get();

    std::string token2 =  _authorisationService.generateToken2(token1Encoded);
    msg = token2 + "\n";
	std::cout << "Sending token2: " << msg << "\n";
    std::cout << "Token size: " << token2.size() << "\n";
    boost::asio::write(socket, boost::asio::buffer(msg));

	
    boost::asio::read_until(socket, buf, '\n');
    msg = std::string(boost::asio::buffer_cast<const char*>(buf.data()),buf.size());
    std::string token3Encoded = msg.substr(0, msg.length() - 1); // remove newline
	std::cout << "Received token3: " << token3Encoded << "\n";
    std::cout << "Token size: " << token3Encoded.size() << "\n";
    std::cin.get();

    _authorisationService.authorizeClient(token3Encoded);

    std::cin.get();
	// Just for testing, echo back messages received from client
    //while (true)
    //{

    //    boost::asio::streambuf buf;
    //    boost::asio::read_until(socket, buf, '\n');
    //    std::string msg = boost::asio::buffer_cast<const char*>(buf.data());
    //    std::cout << "Received: " << msg;

    //    std::string reply = "Server echo: " + msg;
    //    boost::asio::write(socket, boost::asio::buffer(reply));
    //}
}
