#pragma once
#include "AuthorisationServiceAPI.h"
class Server
{
public:
    Server(int _port, AuthorisationServiceAPI& _authorisationService);
};