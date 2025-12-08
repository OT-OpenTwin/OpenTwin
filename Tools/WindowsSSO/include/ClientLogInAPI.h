#pragma once
#include <vector>
#include <string>
class ClientLogInAPI
{
public:
	using SendTokenFunc = std::vector<unsigned char>(*)(std::string&);
	void setSendTokenFunc(SendTokenFunc _func) {
		m_sendTokenFunc = _func;
	}
	void execute(const std::string& _targetName);

	virtual std::vector<unsigned char> generateClientToken(const std::vector<unsigned char>& _inputToken, bool _firstCall) = 0;

private:
	SendTokenFunc m_sendTokenFunc = nullptr; // attribute
};
