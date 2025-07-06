#include "Endpoint.h"

std::string Endpoint::getMessageTypeString() const {
    switch (m_messageType) {
        case mTLS:    return "mTLS";
        case TLS:     return "TLS";
        default:      return "Unknown";
    }
}
