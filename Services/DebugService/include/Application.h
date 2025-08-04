//! @file Application.h
//! @author Alexander Kuester (alexk95)
//! @date March 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// DebugService header
#include "DebugServiceConfig.h"

// OpenTwin header
#include "OTCore/ReturnMessage.h"
#include "OTServiceFoundation/ApplicationBase.h" // Base class

// std header
#include <list>
#include <string>
#include <functional>

class Application : public ot::ApplicationBase {
	OT_DECL_ACTION_HANDLER(Application)
private:
	//! @brief Constructor
	//! Register buttons here!
	Application();

	int m_nameCounter; //! @brief The name counter is used to generate unique debug object names

public:
	// ###########################################################################################################################################################################################################################################################################################################################

	// Button callbacks

	void uiDebugInfo();

	void testHello();
	void testKill();

	void testTableSmall();
	void testTableMedium();
	void testTableBig();
	
	void createPlotOneCurve();
	void createPlotTwoCurves();
	void createFamilyOfCurves();
	void createFamilyOfCurves3ParameterConst();
	void createFamilyOfCurves3Parameter();
	void createPlotScatter();
	void createPlotSinglePoint();

	// ###########################################################################################################################################################################################################################################################################################################################

	// Helper

private:

	void sendTableWorker(int _rows, int _columns);

	// ###########################################################################################################################################################################################################################################################################################################################

	// General feature handling

public:

	DebugServiceConfig m_config;

	void actionAboutToBePerformed(const char* _json);

	void enableFeature(const DebugServiceConfig::FeatureFlag& _flag, bool _enabled = true) { m_config.setFeatureFlag(_flag, _enabled); };
	void disableFeature(const DebugServiceConfig::FeatureFlag& _flag) { m_config.setFeatureFlag(_flag, false); };
	bool getFeatureEnabled(const DebugServiceConfig::FeatureFlag& _flag) const { return m_config.getFeatureFlags().flagIsSet(_flag); };
	
	void setFeatures(const DebugServiceConfig::Features& _features) { m_config.setFeatureFlags(_features); };
	const DebugServiceConfig::Features& getFeatures(void) const { return m_config.getFeatureFlags(); };

	// ###########################################################################################################################################################################################################################################################################################################################
	
	// ###########################################################################################################################################################################################################################################################################################################################
	
	// ###########################################################################################################################################################################################################################################################################################################################

	// Default

	static Application * instance(void);
	static void deleteInstance(void);

private:
	virtual ~Application();

	struct ButtonInfo {
		ButtonInfo(const std::string& _group, const std::string& _buttonNameAndTitle, const std::string& _icon, const std::function<void(void)>& _callback) :
			group(_group), name(_buttonNameAndTitle), title(_buttonNameAndTitle), callback(_callback), icon(_icon)
		{}

		std::string group;
		std::string name;
		std::string title;
		std::string icon;
		std::function<void(void)> callback;
	};

	std::list<ButtonInfo> m_testButtons;

	std::string getButtonKey(const ButtonInfo& _info) const;

public:

	// ##################################################################################################################################################################################################################
		
	OT_HANDLER(handleExecuteModelAction, Application, OT_ACTION_CMD_MODEL_ExecuteAction, ot::SECURE_MESSAGE_TYPES)
	
	// ##################################################################################################################################################################################################################

	// Required functions

	//! @brief Will be called when the service was successfully started, the hppt and/or websocket connection is established and the service may start its work
	virtual void run(void) override;

	//! @brief Will be called whenever a action should be processed. Core actions will be processed in the base and will not be forwarded to this function (see documentation)
	//! @param _action The action that should be processed
	//! @param _doc The document containing all the information
	virtual std::string processAction(const std::string & _action, ot::JsonDocument& _doc) override;

	//! @brief Will be called whenever a message should be processed. Core messages will be processed in the base and will not be forwarded to this function (see documentation)
	//! @param _sender The service that this message was sent from
	//! @param _message The message that should be processed
	//! @param _doc The document containing all the information
	virtual std::string processMessage(ServiceBase* _sender, const std::string& _message, ot::JsonDocument& _doc) override;

	//! @brief Will be called when a UI connected to the session and is ready to work
	virtual void uiConnected(ot::components::UiComponent * _ui) override;

	//! @brief Will be called when a UI disconnected from the session (is already closed)
	virtual void uiDisconnected(const ot::components::UiComponent * _ui) override;

	//! @brief Will be called when a model service connected to the session and is ready to work
	virtual void modelConnected(ot::components::ModelComponent * _model) override;

	//! @brief Will be called when a mode service disconnected from the session (is already closed)
	virtual void modelDisconnected(const ot::components::ModelComponent * _model) override;

	//! @brief Will be called when a service is connected to the session (UI and model services will not trigger this function)
	//! @param _service The service information
	virtual void serviceConnected(ot::ServiceBase * _service) override;

	//! @brief Will be called when a service is disconnected from the session (is already closed) (UI and model services will not trigger this function)
	//! @param _service The service information
	virtual void serviceDisconnected(const ot::ServiceBase * _service) override;

	//! @brief Will be called before the whole session starts to shut down (shut down can not be stopped)
	//! At this point all services, that are listed as connected, are still reachable.
	virtual void preShutdown(void) override;

	//! @brief Will be called when the application is shutting down (shut down can not be stopped)
	virtual void shuttingDown(void) override;

	//! @brief Will return true if this application requires a relay service for a websocket connection
	virtual bool startAsRelayService(void) const override;
	
	//! @brief Create settings that your application uses that are editable in the uiService
	//! The created class will be deleted after used for sending or synchronizing with the database.
	//! The created settings will be requested upon Service startup to synchronize with the database,
	//! aswell as when the uiService is connected
	virtual ot::PropertyGridCfg createSettings(void) const override;

	//! @brief This function will be called when the settings were synchronized with the database
	//! At this point the values from the dataset should be stored since the dataset will be deleted after this function call
	//! @param The dataset that contains all values
	virtual void settingsSynchronized(const ot::PropertyGridCfg& _dataset) override;

	//! @brief This function will be called when the settings were changed in the uiService
	//! The value of the provided item should be stored.
	//! If the change of the item will change the item visibility of any settings item, this function should return true,
	//! otherwise false. When returning true, the function createSettings() will be called and the created dataset will be
	//! send to the uiService to update the Settings in the dialog
	//! @param The item that has been changed in the uiService (instance will be deleted after this function call)
	virtual bool settingChanged(const ot::Property* _item) override;
};
