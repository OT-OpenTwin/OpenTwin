// project header
#include "Endpoint.h"

// std header
#include <iostream>

Endpoint::Endpoint()
    : m_messageType(MessageType::mTLS) {
}

void Endpoint::printDetailedDescription() const {
    for (const std::string& line : m_detailedDescription) {
        std::cout << line << "\n";
    }
}

std::string Endpoint::getMessageTypeString() const {
    switch (m_messageType) {
        case mTLS:    return "mTLS";
        case TLS:     return "TLS";
        default:      return "Unknown";
    }
}

void Endpoint::printEndpoint() const {
    std::cout << "\nPrinting Endpoint: \n";
    std::cout << "================== \n";
    std::cout << "Endpoint name: " << m_name << "\n";
    std::cout << "Endpoint action: " << m_action << "\n";
    std::cout << "Brief description: " << m_briefDescription << "\n";
//    std::cout << "Detailed description: " << m_detailedDescription << "\n";
    std::cout << "Detailed description: " << "\n";
    printDetailedDescription();
    std::cout << "Message type: " << getMessageTypeString() << "\n";
    
    std::cout << "\nParameters: \n";
    std::cout << "----------- \n";
    for (const Parameter& param : m_parameters) {
        param.printParameter();
    }
    
    std::cout << "\nResponse parameters: \n";
    std::cout << "-------------------- \n";
    for (const Parameter& rparam : m_responseParameters) {
        rparam.printParameter();
    }
    std::cout << "Response description: " << m_responseDescription << "\n\n";
}
