
// Service header
#include "stdafx.h"
#include "LibraryManagementWrapper.h"
#include "Application.h"

// OpenTwin header
#include "OTCore/ReturnMessage.h"
#include "DataBase.h"
#include "Connection/ConnectionAPI.h"

std::list<std::string> LibraryManagementWrapper::getCircuitModels() {

	
	
	std::string lmsResonse;
	ot::JsonDocument lmsDocs;

	lmsDocs.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_LMS_GetDocumentList, lmsDocs.GetAllocator()), lmsDocs.GetAllocator());
	lmsDocs.AddMember(OT_ACTION_PARAM_COLLECTION_NAME, ot::JsonString("CircuitModels", lmsDocs.GetAllocator()), lmsDocs.GetAllocator());
	lmsDocs.AddMember(OT_ACTION_PARAM_Type, ot::JsonString("ElementType", lmsDocs.GetAllocator()), lmsDocs.GetAllocator());
	lmsDocs.AddMember(OT_ACTION_PARAM_Value, ot::JsonString("Diode", lmsDocs.GetAllocator()), lmsDocs.GetAllocator());
	lmsDocs.AddMember(OT_PARAM_DB_USERNAME, ot::JsonString(DataBase::GetDataBase()->getUserName(), lmsDocs.GetAllocator()), lmsDocs.GetAllocator());
	lmsDocs.AddMember(OT_PARAM_DB_PASSWORD, ot::JsonString(DataBase::GetDataBase()->getUserPassword(), lmsDocs.GetAllocator()), lmsDocs.GetAllocator());
	lmsDocs.AddMember(OT_ACTION_PARAM_DATABASE_URL, ot::JsonString(DataBase::GetDataBase()->getDataBaseServerURL(), lmsDocs.GetAllocator()), lmsDocs.GetAllocator());
	
	
	// In case of error:
		// Minimum timeout: attempts * thread sleep                  = 30 * 500ms       =   15sec
		// Maximum timeout; attempts * (thread sleep + send timeout) = 30 * (500ms + 3s) = 1.45min
	const int maxCt = 30;
	int ct = 1;
	bool ok = false;

	do {
		lmsResonse.clear();
		if (!(ok = ot::msg::send(Application::instance()->getServiceURL(), m_lmsLocalUrl, ot::EXECUTE, lmsDocs.toJson(), lmsResonse, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit))) {
			OT_LOG_E("Getting Models from LMS failed [Attempt " + std::to_string(ct) + " / " + std::to_string(maxCt) + "]");
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(500ms);

		}
	} while (!ok && ct++ <= maxCt);

	if (!ok) {
		OT_LOG_E("Failed to get Models");
		return {};
	}

	std::list<std::string> listOfModels;

	ot::ReturnMessage rMsg = ot::ReturnMessage::fromJson(lmsResonse);
	if (rMsg != ot::ReturnMessage::Ok) {
		OT_LOG_E("Get Models failed: " + rMsg.getWhat());
		return {};
	}

	ot::JsonDocument models;
	models.fromJson(rMsg.getWhat());

	if (models.IsObject()) {
		ot::ConstJsonObject obj = models.GetConstObject();
		ot::ConstJsonArray docs = obj["Documents"].GetArray();

		for (const ot::JsonValue& val : docs) {
			ot::ConstJsonObject doc = val.GetObject();
			std::string content = doc["Name"].GetString();
			listOfModels.push_back(content);
		}

		return listOfModels;
	}
	else {
		OT_LOG_E("Models is not Json Object");
		return {};
	}
}

std::string LibraryManagementWrapper::getCircuitModel(std::string _modelName) {
	std::string lmsResonse;
	ot::JsonDocument lmsDocs;

	lmsDocs.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_LMS_GetDocument, lmsDocs.GetAllocator()), lmsDocs.GetAllocator());
	lmsDocs.AddMember(OT_ACTION_PARAM_COLLECTION_NAME, ot::JsonString("CircuitModels", lmsDocs.GetAllocator()), lmsDocs.GetAllocator());
	lmsDocs.AddMember(OT_ACTION_PARAM_Type, ot::JsonString("Name", lmsDocs.GetAllocator()), lmsDocs.GetAllocator());
	lmsDocs.AddMember(OT_ACTION_PARAM_Value, ot::JsonString(_modelName, lmsDocs.GetAllocator()), lmsDocs.GetAllocator());
	lmsDocs.AddMember(OT_PARAM_DB_USERNAME, ot::JsonString(DataBase::GetDataBase()->getUserName(), lmsDocs.GetAllocator()), lmsDocs.GetAllocator());
	lmsDocs.AddMember(OT_PARAM_DB_PASSWORD, ot::JsonString(DataBase::GetDataBase()->getUserPassword(), lmsDocs.GetAllocator()), lmsDocs.GetAllocator());
	lmsDocs.AddMember(OT_ACTION_PARAM_DATABASE_URL, ot::JsonString(DataBase::GetDataBase()->getDataBaseServerURL(), lmsDocs.GetAllocator()), lmsDocs.GetAllocator());


	// In case of error:
		// Minimum timeout: attempts * thread sleep                  = 30 * 500ms       =   15sec
		// Maximum timeout; attempts * (thread sleep + send timeout) = 30 * (500ms + 3s) = 1.45min
	const int maxCt = 30;
	int ct = 1;
	bool ok = false;

	do {
		lmsResonse.clear();
		if (!(ok = ot::msg::send(Application::instance()->getServiceURL(), m_lmsLocalUrl, ot::EXECUTE, lmsDocs.toJson(), lmsResonse, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit))) {
			OT_LOG_E("Getting Models from LMS failed [Attempt " + std::to_string(ct) + " / " + std::to_string(maxCt) + "]");
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(500ms);

		}
	} while (!ok && ct++ <= maxCt);

	if (!ok) {
		OT_LOG_E("Failed to get Models");
		return {};
	}

	std::list<std::string> listOfModels;

	ot::ReturnMessage rMsg = ot::ReturnMessage::fromJson(lmsResonse);
	if (rMsg != ot::ReturnMessage::Ok) {
		OT_LOG_E("Get Models failed: " + rMsg.getWhat());
		return {};
	}

	ot::JsonDocument models;
	models.fromJson(rMsg.getWhat());

	return rMsg.getWhat();	
}

std::string LibraryManagementWrapper::requestCreateConfig(ot::JsonDocument& _doc) {
	std::string lmsResonse;
	

	// In case of error:
		// Minimum timeout: attempts * thread sleep                  = 30 * 500ms       =   15sec
		// Maximum timeout; attempts * (thread sleep + send timeout) = 30 * (500ms + 3s) = 1.45min
	const int maxCt = 30;
	int ct = 1;
	bool ok = false;

	do {
		lmsResonse.clear();
		if (!(ok = ot::msg::send(Application::instance()->getServiceURL(), m_lmsLocalUrl, ot::EXECUTE, _doc.toJson(), lmsResonse, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit))) {
			OT_LOG_E("Getting Models from LMS failed [Attempt " + std::to_string(ct) + " / " + std::to_string(maxCt) + "]");
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(500ms);

		}
	} while (!ok && ct++ <= maxCt);

	if (!ok) {
		OT_LOG_E("Failed to get Models");
		return {};
	}

	std::list<std::string> listOfModels;

	ot::ReturnMessage rMsg = ot::ReturnMessage::fromJson(lmsResonse);
	if (rMsg != ot::ReturnMessage::Ok) {
		OT_LOG_E("Get Models failed: " + rMsg.getWhat());
		return {};
	}

	return rMsg.getWhat();
}
