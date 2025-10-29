// @otlicense

#pragma once
/*********************************************************************
 * @file   PortFactory.h
 * @brief  Creates a port depending on a portsettings object
 * 
 * @author Jan Wagner
 * @date   August 2022
 *********************************************************************/
#include "Port.h"
#include "SolverSettings/PortSettings.h"

class PortFactory
{
public:

	Port<float> * CreatePort(PortSettings settings, std::pair<Point3D, index_t> sourceLocation);

private:

};
