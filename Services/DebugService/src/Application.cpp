//! @file Application.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2025
// ###########################################################################################################################################################################################################################################################################################################################

// DebugService header
#include "Application.h"

// OpenTwin header
#include "OTCore/RuntimeTests.h"
#include "OTCore/ReturnMessage.h"
#include "OTCore/ThisComputerInfo.h"
#include "OTGui/TableCfg.h"
#include "OTServiceFoundation/UiComponent.h"
#include "OTServiceFoundation/ModelComponent.h"
#include "OTServiceFoundation/AbstractUiNotifier.h"
#include "OTServiceFoundation/AbstractModelNotifier.h"
#include "OTCommunication/Msg.h"
#include "OTCommunication/ActionTypes.h"

// std header
#include <thread>

#define OT_DEBUG_SERVICE_PAGE_NAME "Debug"

Application::Application() :
	ot::ApplicationBase(OT_INFO_SERVICE_TYPE_DebugService, OT_INFO_SERVICE_TYPE_DebugService, new ot::AbstractUiNotifier(), new ot::AbstractModelNotifier()),
	m_nameCounter(0)
{
	// Add buttons here
	m_testButtons.push_back(ButtonInfo("Test", "Info", "Information", std::bind(&Application::testHello, this)));
	m_testButtons.push_back(ButtonInfo("Test", "Kill", "Kill", std::bind(&Application::testKill, this)));
	
	m_testButtons.push_back(ButtonInfo("Table", "Small (100k)", "GreenCircle", std::bind(&Application::testTableSmall, this)));
	m_testButtons.push_back(ButtonInfo("Table", "Medium (1M)", "YellowCircle", std::bind(&Application::testTableMedium, this)));
	m_testButtons.push_back(ButtonInfo("Table", "Big (10M)", "RedCircle", std::bind(&Application::testTableBig, this)));

	// Enable features (Exit)
	
	//this->enableFeature(DebugServiceConfig::ExitOnInit, true);
	//this->enableFeature(DebugServiceConfig::ExitOnRun, true);
	//this->enableFeature(DebugServiceConfig::ExitOnPing, true);
	//this->enableFeature(DebugServiceConfig::ExitOnPreShutdown, true);
	//this->enableFeature(DebugServiceConfig::ExitOnHello, true);
		
	// Enable features (Other)

	//this->enableFeature(DebugServiceConfig::ExportOnStart, true);

	// --------------------------------------------------------------------------------------------------------+

	// If exit on init is enabled, exit the service
	if (this->getFeatureEnabled(DebugServiceConfig::ExitOnInit)) {
		OT_LOG_T("Performing exit on init");
		exit(0);
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Button callbacks

void Application::testHello(void) {
	if (this->getFeatureEnabled(DebugServiceConfig::FeatureFlag::ExitOnHello)) {
		OT_LOG_T("Performing exit on \"hello\"");
		exit(0);
	}

	OT_LOG_T("Hello :-)\n\nThis Computer Info:\n" + ot::ThisComputerInfo::toInfoString(ot::ThisComputerInfo::GatherAllMode));
}

void Application::testKill(void) {
	std::thread t([]() {
		OT_LOG_T("Bye :-)");
		exit(0);
		}
	);
	t.detach();
}

void Application::testTableSmall(void) {
	std::thread t(&Application::sendTableWorker, this, 1000, 100);
	t.detach();
}

void Application::testTableMedium(void) {
	std::thread t(&Application::sendTableWorker, this, 10000, 100);
	t.detach();
}

void Application::testTableBig(void) {
	std::thread t(&Application::sendTableWorker, this, 10000, 1000);
	t.detach();
}

void Application::sendTableWorker(int _rows, int _columns) {
	using namespace ot;

	auto ui = this->uiComponent();
	if (!ui) {
		OT_LOG_E("No ui? How?");
		return;
	}

	// Check interval
	RuntimeIntervalTest intervalTest;

	// Display start info
	StyledTextBuilder startInfo;
	startInfo << "Test table with " + std::to_string(_rows) + " rows and " + std::to_string(_columns) + " columns. Started at " << StyledText::TimeHHMMSSZZZZ << " (user computer time).";
	ui->displayStyledMessage(startInfo);

	// Create table document
	JsonDocument doc;
	this->getBasicServiceInformation().addToJsonObject(doc, doc.GetAllocator());

	doc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_UI_TABLE_Setup, doc.GetAllocator()), doc.GetAllocator());

	// Fill cells with some default values
	TableCfg cfg(_rows, _columns);
	for (int r = 0; r < _rows; r++) {
		for (int c = 0; c < _columns; c++) {
			cfg.setCellText(r, c, "Cell " + std::to_string(r) + ":" + std::to_string(c));
		}
	}
	cfg.setEntityName("Test (" + std::to_string(m_nameCounter++) + "): Table");
	cfg.setCellText(0, 0, "Cell count: " + std::to_string(_rows * _columns));

	JsonObject cfgObj;
	cfg.addToJsonObject(cfgObj, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_Config, cfgObj, doc.GetAllocator());

	std::string resp;
	ui->sendMessage(true, doc, resp);

	// Display end info
	StyledTextBuilder endInfo;
	endInfo << "Table test finished at " << StyledText::TimeHHMMSSZZZZ << " (user computer time). Debug service needed " + intervalTest.currentIntervalString() + " to handle the button event.";
	ui->displayStyledMessage(endInfo);
}

// ###########################################################################################################################################################################################################################################################################################################################

// General feature handling

void Application::actionAboutToBePerformed(const char* _json) {
	ot::JsonDocument doc;
	if (!doc.fromJson(_json)) {
		OT_LOG_EAS("Failed to deserialize request: \"" + std::string(_json) + "\"");
		return;
	}

	std::string action = ot::json::getString(doc, OT_ACTION_MEMBER);

	if (action == OT_ACTION_CMD_Ping) {
		if (this->getFeatureEnabled(DebugServiceConfig::FeatureFlag::ExitOnPing)) {
			OT_LOG_T("Performing exit on ping");
			exit(0);
		}
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// Default methods

Application* g_instance{ nullptr };

Application * Application::instance(void) {
	if (g_instance == nullptr) { g_instance = new Application; }
	return g_instance;
}

void Application::deleteInstance(void) {
	if (g_instance) { delete g_instance; }
	g_instance = nullptr;
}

Application::~Application()
{

}

std::string Application::handleExecuteModelAction(ot::JsonDocument& _document) {
	std::string action = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_ActionName);

	ButtonInfo* info = nullptr;
	for (ButtonInfo& btn : m_testButtons) {
		if (action == this->getButtonKey(btn)) {
			btn.callback();
			return std::string();
		}
	}
	
	OT_LOG_WAS("Unknown model action \"" + action + "\"");
	return std::string();
}

// ##################################################################################################################################################################################################################

std::string Application::getButtonKey(const ButtonInfo& _info) const {
	return OT_DEBUG_SERVICE_PAGE_NAME ":" + _info.group + ":" + _info.name;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Required functions

void Application::run(void) {
	if (this->getFeatureEnabled(DebugServiceConfig::FeatureFlag::ExitOnRun)) {
		OT_LOG_T("Performing exit on run");
		exit(0);
	}


}

std::string Application::processAction(const std::string & _action, ot::JsonDocument & _doc)
{
	return ""; // Return empty string if the request does not expect a return
}

std::string Application::processMessage(ServiceBase * _sender, const std::string & _message, ot::JsonDocument & _doc)
{
	return ""; // Return empty string if the request does not expect a return
}

void Application::uiConnected(ot::components::UiComponent * _ui)
{
	enableMessageQueuing(OT_INFO_SERVICE_TYPE_UI, true);

	_ui->addMenuPage(OT_DEBUG_SERVICE_PAGE_NAME);

	std::list<std::string> addedGroups;
	for (const auto& it : m_testButtons) {
		// Add group if needed
		if (std::find(addedGroups.begin(), addedGroups.end(), it.group) == addedGroups.end()) {
			_ui->addMenuGroup(OT_DEBUG_SERVICE_PAGE_NAME, it.group);
			addedGroups.push_back(it.group);
		}

		_ui->addMenuButton(OT_DEBUG_SERVICE_PAGE_NAME, it.group, it.name, it.title, ot::LockModelWrite | ot::LockViewRead | ot::LockViewWrite, it.icon);
	}

	enableMessageQueuing(OT_INFO_SERVICE_TYPE_UI, false);
}

void Application::uiDisconnected(const ot::components::UiComponent * _ui)
{

}

void Application::modelConnected(ot::components::ModelComponent * _model)
{

}

void Application::modelDisconnected(const ot::components::ModelComponent * _model)
{

}

void Application::serviceConnected(ot::ServiceBase * _service)
{

}

void Application::serviceDisconnected(const ot::ServiceBase * _service)
{

}

void Application::preShutdown(void) {
	if (this->getFeatureEnabled(DebugServiceConfig::FeatureFlag::ExitOnPreShutdown)) {
		OT_LOG_T("Performing exit on pre shutdown");
		exit(0);
	}
}

void Application::shuttingDown(void)
{

}

bool Application::startAsRelayService(void) const
{
	return false;	// Do not want the service to start a relay service. Otherwise change to true
}

ot::PropertyGridCfg Application::createSettings(void) const {
	return ot::PropertyGridCfg();
}

void Application::settingsSynchronized(const ot::PropertyGridCfg& _dataset) {

}

bool Application::settingChanged(const ot::Property * _item) {
	return false;
}

// ###########################################################################################################################################################################################################################################################################################################################
