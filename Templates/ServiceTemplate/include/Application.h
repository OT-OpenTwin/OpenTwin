//! @file Application.h
//! @author 
//! @date 
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// Open twin header
#include "OTServiceFoundation/ApplicationBase.h" // Base class

// C++ header
#include <string>

// Forward declaration
namespace ot {
	namespace components {
		class UiComponent;
		class ModelComponent;
	}
}

class Application : public ot::ApplicationBase {
public:
	static Application& instance();

private:
	Application();
	virtual ~Application();
public:

	// ###########################################################################################################################################################################################################################################################################################################################

	// Add your custom functions/ members here

	// A handler can be created to handle the the specified action
	// In this example the first parameter is the name of the callback function
	// The second parameter is the class name where the handler is created at
	// The third parameter is a String containing the action name
	// The last parameter are flags describing the allowed message types for this handler
	//OT_HANDLER(myHandleFunctionName, Application, "actionToHandle", ot::SECURE_MESSAGE_TYPES);

};
