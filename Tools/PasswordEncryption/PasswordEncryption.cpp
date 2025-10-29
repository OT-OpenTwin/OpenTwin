// @otlicense

#include <iostream>
#include <string>

#include "OTServiceFoundation/UserCredentials.h"

int main()
{
	std::string pwd;
	
	std::cout << "Enter Password: ";
	std::cin >> pwd;

	ot::UserCredentials userCredential;
	userCredential.setPassword(pwd);

	std::cout << "Encrypted password: " << userCredential.getEncryptedPassword() << std::endl;

	return 0;
}
