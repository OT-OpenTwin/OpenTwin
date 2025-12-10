// @otlicense
// File: Client.cpp
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

#include "Client.h"

#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include "Base64Encoding.h"
using boost::asio::ip::tcp;

Client::Client(const std::string& _ip, int _port, ClientLogInAPI& _clientLogIn)
{
    boost::asio::io_context io;

    tcp::socket socket (io);

    tcp::endpoint endpoint(
        boost::asio::ip::make_address(_ip),
        _port
    );
    std::cout << "Connecting to server " + _ip + ":" + std::to_string(_port) + "\n";
   socket.connect(endpoint);

   std::cout << "Connection established";

   std::vector<unsigned char> token1 = _clientLogIn.generateClientToken({}, true);
   const std::string token1_enc = encode(token1);
   std::string msg = token1_enc +  "\n";  // protocol: newline terminated
   std::cout << "Sending token1: " << token1_enc << "\n";
   std::cout << "Token size: " << token1_enc.size() << "\n";
   boost::asio::write(socket, boost::asio::buffer(msg));
   std::cout << "Token1 sent.\n";

   boost::asio::streambuf buf;
   boost::asio::read_until(socket, buf, '\n');
   std::string reply(boost::asio::buffer_cast<const char*>(buf.data()),buf.size());
   std::string token2Encoded = reply.substr(0, reply.length() - 1); // remove newline
   
   std::cout << "Received token2: " << token2Encoded << "\n";
   std::cout << "Token size: " << token2Encoded.size() << "\n";
   std::cin.get();

   std::vector<unsigned char> token2 = decode(token2Encoded);
   std::vector<unsigned char> token3 = _clientLogIn.generateClientToken(token2, false);
   const std::string token3_enc = encode(token3);
   msg = token3_enc + "\n";  // protocol: newline terminated
   std::cout << "Sending token3: " << token3_enc << "\n"; 
   std::cout << "Token size: " << token3_enc.size() << "\n";
   boost::asio::write(socket, boost::asio::buffer(msg));
   std::cout << "Token3 sent.\n";

   std::cin.get();
   // Just for testing, keep the client alive to see server logs
    //for (;;) {
    //    std::string msg;
    //    std::cout << "Enter message: ";
    //    std::getline(std::cin, msg);

    //    msg += "\n";  // protocol: newline terminated
    //    boost::asio::write(socket, boost::asio::buffer(msg));

    //    // Read server reply
    //    boost::asio::streambuf buf;
    //    boost::asio::read_until(socket, buf, '\n');
    //    std::string reply = boost::asio::buffer_cast<const char*>(buf.data());

    //    std::cout << "Reply: " << reply;
    //}

}
