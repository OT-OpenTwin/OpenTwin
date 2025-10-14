// Open Twin header
#include "OTCore/LogDispatcher.h"

// project header
#include "Service.h"

// std header
#include <iostream>

void Service::printService() const {
    OT_LOG_D("Printing Service: " + m_name);
    OT_LOG_D("==================");
    OT_LOG_D("Number of endpoints in service: " + std::to_string(getEndpoints().size()));
    OT_LOG_D("Endpoint actions:");
    for (const Endpoint& endpoint : m_endpoints) {
        OT_LOG_D(endpoint.getAction());
    }
}
