#include "AuthorisationService.h"
#include "ClientLogIn.h"
#include "Client.h"
#include "Server.h"
#include "Base64Encoding.h"
  
int main()
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
    int32_t port = 5555;
    #ifdef SERVER
        AuthorisationService authorisationService;
	    Server server(port, authorisationService);
        
    #else
        ClientLogIn clientLogin;
	    Client client(port, clientLogin);
    #endif 

#endif // DEBUG

    return 0;
}
