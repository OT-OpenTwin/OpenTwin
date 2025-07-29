// project header
#include "Service.h"

// std header
#include <iostream>

void Service::printService() const {
    std::cout << "\nPrinting Service: " << m_name << "\n";
    std::cout << "================== \n";
    std::cout << "Endpoint actions:\n";
    for (const Endpoint& endpoint : m_endpoints) {
        std::cout << endpoint.getAction() << "\n";
    }
}
