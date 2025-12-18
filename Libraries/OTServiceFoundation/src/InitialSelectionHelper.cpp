// @otlicense

// OpenTwin header
#include "OTSystem/DateTime.h"
#include "OTCore/LogDispatcher.h"
#include "OTCore/ReturnMessage.h"
#include "OTModelAPI/ModelServiceAPI.h"
#include "OTCommunication/ActionTypes.h"
#include "OTServiceFoundation/InitialSelectionHelper.h"
#include "EntityInformation.h"

ot::InitialSelectionHelper::InitialSelectionHelper()
	: m_mode(Undefined), m_sessionOpenTimeout(5 * ot::msg::defaultTimeout), m_running(false) {

}

ot::InitialSelectionHelper::~InitialSelectionHelper() {
	reset();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter

void ot::InitialSelectionHelper::setSessionOpenTimeout(int64_t _timeout) {
	std::lock_guard<std::mutex> lock(m_mutex);
	m_sessionOpenTimeout = _timeout;
}

void ot::InitialSelectionHelper::setMode(Mode _mode) {
	std::lock_guard<std::mutex> lock(m_mutex);
	m_mode = _mode;
}

void ot::InitialSelectionHelper::setEntityName(const std::string& _entityName) {
	std::lock_guard<std::mutex> lock(m_mutex);
	m_entityName = _entityName;
}

void ot::InitialSelectionHelper::setModelUrl(const std::string& _modelUrl) {
	std::lock_guard<std::mutex> lock(m_mutex);
	m_modelUrl = _modelUrl;
}

void ot::InitialSelectionHelper::setUiUrl(const std::string& _uiUrl) {
	std::lock_guard<std::mutex> lock(m_mutex);
	m_uiUrl = _uiUrl;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Getter

bool ot::InitialSelectionHelper::isReady() {
	std::lock_guard<std::mutex> lock(m_mutex);
	return m_mode != Undefined && !m_entityName.empty() && !m_modelUrl.empty() && !m_uiUrl.empty();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Behavior

bool ot::InitialSelectionHelper::runIfReady() {
	if (!isReady()) {
		return false;
	}
	if (m_running) {
		OT_LOG_W("InitialSelectionHelper is already running");
		return false;
	}

	m_running = true;

	std::thread workerThread(&InitialSelectionHelper::worker, this);
	workerThread.detach();

	return true;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Worker

void ot::InitialSelectionHelper::reset() {
	std::lock_guard<std::mutex> lock(m_mutex);
	m_mode = Undefined;
	m_uiUrl.clear();
	m_modelUrl.clear();
	m_entityName.clear();
	m_running = false;
}

void ot::InitialSelectionHelper::worker() {
	auto initialTime = ot::DateTime::msSinceEpoch();

	// Ensure that the model is ready
	bool modelReady = false;
	while (!modelReady) {
		if (!m_running) {
			return;
		}

		if (!isReady()) {
			OT_LOG_W("InitialSelectionHelper configuration changed while waiting for model to open the project. Aborting...");
			m_running = false;
			return;
		}

		// Check for timeout
		if (isTimeout(initialTime)) {
			OT_LOG_E("Timeout while waiting for model to open the project");
			m_running = false;
			exit(ot::AppExitCode::GeneralTimeout);
			return;
		}

		// Check if model is ready
		if (checkModelReady()) {
			modelReady = true;
		}
		else {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}

	// If we reach this point, the model is ready

	// Determine entity to select
	std::string entityToSelect = determineEntityToSelect();

	if (entityToSelect.empty()) {
		// No entity to select, reset and abort
		reset();
		return;
	}

	if (!m_running) {
		return;
	}

	// Send initial selection to UI
	sendInitialSelectionToUI(entityToSelect);

	// Finally reset the helper
	reset();
}

bool ot::InitialSelectionHelper::isTimeout(int64_t _initialTime) {
	std::lock_guard<std::mutex> lock(m_mutex);
	auto currentTime = ot::DateTime::msSinceEpoch();
	return ((currentTime - _initialTime) > m_sessionOpenTimeout);
}

bool ot::InitialSelectionHelper::checkModelReady() {
	std::lock_guard<std::mutex> lock(m_mutex);

	if (m_modelUrl.empty()) {
		OT_LOG_W("Model URL is empty, cannot check if project is open");
		return false;
	}

	std::string responseStr;
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_IsProjectOpen, doc.GetAllocator()), doc.GetAllocator());

	// Send request. We can ignore the success since the application will exit on send failed	
	ot::msg::send("", m_modelUrl, ot::EXECUTE, doc.toJson(), responseStr);

	ot::ReturnMessage response = ot::ReturnMessage::fromJson(responseStr);
	return (response == ot::ReturnMessage::True);
}

void ot::InitialSelectionHelper::sendInitialSelectionToUI(const std::string& _entityToSelect) {
	std::lock_guard<std::mutex> lock(m_mutex);

	if (m_uiUrl.empty()) {
		OT_LOG_W("UI URL is empty, cannot send initial selection");
		return;
	}

	ot::JsonDocument uiDoc;
	uiDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_SetEntitySelected, uiDoc.GetAllocator()), uiDoc.GetAllocator());
	uiDoc.AddMember(OT_ACTION_PARAM_IsSelected, true, uiDoc.GetAllocator());
	uiDoc.AddMember(OT_ACTION_PARAM_ClearSelection, true, uiDoc.GetAllocator());
	uiDoc.AddMember(OT_ACTION_PARAM_NAME, ot::JsonString(_entityToSelect, uiDoc.GetAllocator()), uiDoc.GetAllocator());

	std::string tmp;
	// We can ignore the return code since the application will exit on send failed
	ot::msg::send("", m_uiUrl, ot::QUEUE, uiDoc.toJson(), tmp);
}

std::string ot::InitialSelectionHelper::determineEntityToSelect() {
	std::lock_guard<std::mutex> lock(m_mutex);

	if (m_modelUrl.empty()) {
		OT_LOG_W("Model URL is empty, cannot determine entity to select");
		return std::string();
	}
	if (m_entityName.empty()) {
		OT_LOG_W("Entity name is empty, cannot determine entity to select");
		return std::string();
	}

	switch (m_mode) {
	case ot::InitialSelectionHelper::Undefined:
		OT_LOG_E("InitialSelectionHelper mode is undefined");
		return std::string();

	case ot::InitialSelectionHelper::SelectByName:
		return ensureEntityExistsByName(m_entityName);

	case ot::InitialSelectionHelper::SelectFirstChild:
		return determineFirstChildEntity(m_entityName);

	default:
		OT_LOG_E("Unknown mode set (" + std::to_string(m_mode) + ")");
		return std::string();
	}
}

std::string ot::InitialSelectionHelper::ensureEntityExistsByName(const std::string& _entityName) {
	ot::EntityInformation info;
	if (!ot::ModelServiceAPI::getEntityInformation(_entityName, info)) {
		return std::string();
	}
	return info.getEntityName();
}

std::string ot::InitialSelectionHelper::determineFirstChildEntity(const std::string& _parentEntity) {
	std::list<std::string> listOfChilds = ot::ModelServiceAPI::getListOfFolderItems(_parentEntity, false);

	if (listOfChilds.empty()) {
		return std::string();
	}
	else {
		return listOfChilds.front();
	}
}
