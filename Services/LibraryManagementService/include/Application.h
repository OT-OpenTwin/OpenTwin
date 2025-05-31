//! @file Application.h
//! @author Sebastian Urmann
//! @date May 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once
// Service header
#include "MongoWrapper.h"

// OpenTwin header
#include "OTCore/ServiceBase.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/ActionHandler.h"


// std header
#include <list>
#include <string>

class Application : public ot::ServiceBase {
	OT_DECL_ACTION_HANDLER(Application)
public:
	static Application& instance(void);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Public functions
	int initialize(const char* _ownURL, const char* _globalSessionServiceURL, const char* _databasePWD);
	

	
	

	

private:

	// ###########################################################################################################################################################################################################################################################################################################################

	// Action handler
	OT_HANDLER(handleGetDocument, Application, OT_ACTION_CMD_LMS_GetDocument, ot::SECURE_MESSAGE_TYPES);
	

	// ###########################################################################################################################################################################################################################################################################################################################
	
	MongoWrapper* db = nullptr;
	// Constructor/Destructor

	Application();
	virtual ~Application();

	
};
