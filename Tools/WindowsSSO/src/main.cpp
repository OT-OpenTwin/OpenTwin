#include "AuthorisationService.h"
#include "Client.h"


int main()
{
    Client client;

    // -------- Generate Token1 --------
    std::vector<BYTE> token1 = client.generateClientToken({}, true);
    const std::string token1Decoded = encode(token1);
    
    // send token1 to server
    
    // receive token2 from server into token2 vector
    AuthorisationService authorisationService;
    std::string token2Decoded = authorisationService.generateToken2(token1Decoded);
    std::vector<BYTE>  token2 = decode(token2Decoded);

    // -------- Generate Token3 --------
    std::vector<BYTE> token3 = client.generateClientToken(token2, false);

	const std::string token3Decoded = encode(token3);
	authorisationService.authorizeClient(token3Decoded);

    
    return 0;
}
