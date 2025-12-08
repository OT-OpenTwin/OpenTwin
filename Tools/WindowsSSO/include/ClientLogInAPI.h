#pragma once
#include <vector>

class ClientLogInAPI
{
public:
	
	virtual std::vector<unsigned char> generateClientToken(const std::vector<unsigned char>& _inputToken, bool _firstCall) = 0;
};
