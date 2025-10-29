// @otlicense

//! @file LogNotifierStdCout.cpp
//! @author Alexander Kuester (alexk95)
//! @date January 2021
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/LogNotifierStdCout.h"

// std header
#include <iostream>

void ot::LogNotifierStdCout::log(const LogMessage& _message) {
	std::cout << _message << std::endl;
}
