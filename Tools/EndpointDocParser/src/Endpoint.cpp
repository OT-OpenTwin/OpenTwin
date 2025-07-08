#include "Endpoint.h"

// std header
#include <iostream>

std::string Endpoint::getMessageTypeString() const {
    switch (m_messageType) {
        case mTLS:    return "mTLS";
        case TLS:     return "TLS";
        default:      return "Unknown";
    }
}

void Endpoint::printEndpoint() const {
    std::cout << "Printing Endpoint: \n";
    std::cout << "Endpoint name: " << m_name << "\n";
    std::cout << "Endpoint action: " << m_action << "\n";
    std::cout << "Brief description: " << m_briefDescription << "\n";
    std::cout << "Detailed description: " << m_detailedDescription << "\n";
    std::cout << "Message type: " << m_messageType << "\n";
    
    std::cout << "Parameters: \n";
    for (const Parameter& param : m_parameters) {
        param.printParameter();
    }
    
    std::cout << "Response parameters: \n";
    for (const Parameter& rparam : m_responseParameters) {
        rparam.printParameter();
    }
    std::cout << "Response description: " << m_responseDescription << "\n";
}
