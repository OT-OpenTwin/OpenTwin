#pragma once
#include <string>

class AuthorisationServiceAPI
{
public:
	virtual std::string generateToken2(const std::string& _token1) = 0;
	virtual void authorizeClient(const std::string& _token3) = 0;
};
