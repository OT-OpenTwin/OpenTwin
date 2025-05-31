//! @file Application.cpp
//! @author Sebastian Urmann
//! @date May 2025
// ###########################################################################################################################################################################################################################################################################################################################

// LDS header
#include "Application.h"

// OpenTwin header#
#include "OTSystem/AppExitCodes.h"
#include "OTCore/Logger.h"
#include "OTCommunication/Msg.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/ActionDispatcher.h"
#include "OTCore/ReturnMessage.h"

//std header
#include <chrono>
#include <thread>

Application& Application::instance(void) {
	static Application g_instance;
	return g_instance;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Public functions

int Application::initialize(const char* _ownURL, const char* _globalSessionServiceURL, const char * _databasePWD) {
	try {
		OT_LOG_I("Library Management Service initialization");

		// Now store the command line arguments and perform the initialization
		if (_ownURL == nullptr) {
			exit(ot::AppExitCode::ServiceUrlInvalid);
		}
		if (_globalSessionServiceURL == nullptr) {
			exit(ot::AppExitCode::GSSUrlMissing);
		}

		std::string ownUrl(_ownURL);

		// Register at GSS
		ot::JsonDocument gssDoc;
		gssDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_RegisterNewLibraryManagementService, gssDoc.GetAllocator()), gssDoc.GetAllocator());
		gssDoc.AddMember(OT_ACTION_PARAM_LIBRARYMANAGEMENT_SERVICE_URL, ot::JsonString(ownUrl, gssDoc.GetAllocator()), gssDoc.GetAllocator());

		// Send request to GSS
			std::string gssResponse;

		// In case of error:
		// Minimum timeout: attempts * thread sleep                  = 30 * 500ms       =   15sec
		// Maximum timeout; attempts * (thread sleep + send timeout) = 30 * (500ms + 3s) = 1.45min
		const int maxCt = 30;
		int ct = 1;
		bool ok = false;
	
		do {
			gssResponse.clear();
			if (!(ok = ot::msg::send(ownUrl, _globalSessionServiceURL, ot::EXECUTE, gssDoc.toJson(), gssResponse, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit))) {
				OT_LOG_E("Register at Global Session Service failed [Attempt " + std::to_string(ct) + " / " + std::to_string(maxCt) + "]");
				using namespace std::chrono_literals;
				std::this_thread::sleep_for(500ms);
				
			}
		} while (!ok && ct++ <= maxCt);

		if (!ok) {
			OT_LOG_E("Registration at Global Session service failed after " + std::to_string(maxCt) + " attemts. Exiting...");
			exit(ot::AppExitCode::GSSRegistrationFailed);
		}

		ot::ReturnMessage rMsg = ot::ReturnMessage::fromJson(gssResponse);
		if (rMsg != ot::ReturnMessage::Ok) {
			OT_LOG_E("Registration failed: " + rMsg.getWhat());
			exit(ot::AppExitCode::GSSRegistrationFailed);
		}

		// Get DBUrl and AuthUrl from gss response

		ot::JsonDocument gssRespoonseUrls;
		gssRespoonseUrls.fromJson(rMsg.getWhat());

		std::string dbUrl = ot::json::getString(gssRespoonseUrls, OT_ACTION_PARAM_SERVICE_DBURL);
		std::string authUrl = ot::json::getString(gssRespoonseUrls, OT_ACTION_PARAM_SERVICE_AUTHURL);
				
		// Now we need password and we get it from authorisation Service
		//Authorisation has no password so I will to it currently like in authorisation service by command argument 


		// Initialzation of MongoDB connection
		db = new MongoWrapper(dbUrl, _databasePWD);



	}
	catch (std::exception& e) {
		OT_LOG_EAS(std::string{"Uncaught exception: "}.append(e.what()));
		exit(ot::AppExitCode::GeneralError);
	}
	catch (...) {
		assert(0);
		exit(ot::AppExitCode::UnknownError);
	}

	
	
	
	OT_LOG_D("Initialization finished");
	return ot::AppExitCode::Success;
}


// ###########################################################################################################################################################################################################################################################################################################################

// Action handler

std::string Application::handleGetDocument(ot::JsonDocument& _document) {

	std::string collectionName= ot::json::getString(_document, OT_PARAM_AUTH_COLLECTION_NAME);
	std::string name = ot::json::getString(_document, OT_ACTION_PARAM_NAME);


	auto result = db->getDocument(collectionName,name);
	if (result) {
		
		return ot::ReturnMessage(ot::ReturnMessage::Ok, bsoncxx::to_json(*result)).toJson();
	}
	else {
		return ot::ReturnMessage(ot::ReturnMessage::Failed).toJson();
	}


}

// ###########################################################################################################################################################################################################################################################################################################################

// Constructor/Destructor

Application::Application() :
	ot::ServiceBase(OT_INFO_SERVICE_TYPE_LibraryManagementService, OT_INFO_SERVICE_TYPE_LibraryManagementService) {

}

Application::~Application() {}
