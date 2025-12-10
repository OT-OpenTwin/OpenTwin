// @otlicense
// File: main.cpp
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

#include "AuthorisationService.h"
#include "ClientLogIn.h"
#include "Client.h"
#include "Server.h"
#include "Base64Encoding.h"
 
int main(int argc, char* argv[])
{
#ifndef DISTRIBUTED
        ClientLogIn client;

        // -------- Generate Token1 --------
        std::vector<BYTE> token1 = client.generateClientToken({}, true);
        const std::string token1Encoded = encode(token1);
    
        // send token1 to server
    
        // receive token2 from server into token2 vector
        AuthorisationService authorisationService;
        std::string token2Decoded = authorisationService.generateToken2(token1Encoded);
        std::vector<BYTE>  token2 = decode(token2Decoded);

        // -------- Generate Token3 --------
        std::vector<BYTE> token3 = client.generateClientToken(token2, false);

	    const std::string token3Decoded = encode(token3);
	    authorisationService.authorizeClient(token3Decoded);
    
#else
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <ip>:string <port>:int32\n";
        return 1;
    }

    // First argument: string
    std::string ip = argv[1];

    // Second argument: int32_t
    int32_t port = static_cast<int32_t>(std::strtol(argv[2], nullptr, 10));

    #ifdef SERVER
        AuthorisationService authorisationService;
	    Server server(ip,port, authorisationService);
        
    #else
        ClientLogIn clientLogin;
	    Client client(ip,port, clientLogin);
    #endif 

#endif // DEBUG

    return 0;
}
