// @otlicense

// Service header
#include "Application.h"
#include "ModelNotifier.h"
#include "UiNotifier.h"

// Open twin header
#include "OTServiceFoundation/UiComponent.h"
#include "OTServiceFoundation/ModelComponent.h"

Application& Application::instance() {
	static Application g_instance;
	return g_instance;
}

Application::Application()
	: ot::ApplicationBase(<MY_SERVICE_NAME>, <MY_SERVICE_TYPE>, new UiNotifier(), new ModelNotifier())
{
	
}

Application::~Application()
{

}

// ###########################################################################################################################################################################################################################################################################################################################

// Custom functions


