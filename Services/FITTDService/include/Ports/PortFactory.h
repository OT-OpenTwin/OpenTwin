// @otlicense

#pragma once

#include "Port.h"
#include "SolverSettings/PortSettings.h"

//! @brief Creates a port depending on a portsettings object
class PortFactory
{
public:

	Port<float> * CreatePort(PortSettings settings, std::pair<Point3D, index_t> sourceLocation);

private:

};
