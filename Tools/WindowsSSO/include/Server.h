#pragma once
#include "AuthorisationServiceAPI.h"
#include <string>
class Server
{
public:
    Server(const std::string& _ip, int _port, AuthorisationServiceAPI& _authorisationService);
};