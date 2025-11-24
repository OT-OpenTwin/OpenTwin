#pragma once
#include "ClientLogInAPI.h"
#include <string>
class Client
{
public:
    Client(const std::string& _ip, int _port, ClientLogInAPI& _clientLogIn);
};
