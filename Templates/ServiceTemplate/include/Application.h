//! @file Application.h
//! @author 
//! @date 
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTServiceFoundation/ApplicationBase.h" // Base class

class Application : public ot::ApplicationBase {
public:
	static Application& instance();

private:
	Application();
	virtual ~Application();
public:


};
