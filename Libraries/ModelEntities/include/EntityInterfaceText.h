#pragma once
#include <string>

class __declspec(dllexport) EntityInterfaceText
{
public:
	virtual std::string getText(void) = 0;
};
