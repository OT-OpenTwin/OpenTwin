#pragma once
#include <exception>

class ExceptionRestartRequired : public std::exception
{
public:
	ExceptionRestartRequired() = default;
};
