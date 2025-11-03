// @otlicense
// File: Application.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#pragma once

// DebugService header
#include "DebugServiceConfig.h"

// OpenTwin header
#include "OTCore/ReturnMessage.h"
#include "OTGuiAPI/ButtonHandler.h"
#include "OTServiceFoundation/ApplicationBase.h" // Base class

// std header
#include <list>
#include <string>
#include <functional>

class Application : public ot::ApplicationBase, public ot::ButtonHandler {
private:
	//! @brief Constructor
	//! Register buttons here!
	Application();

	int m_nameCounter; //! @brief The name counter is used to generate unique debug object names

public:
	// ###########################################################################################################################################################################################################################################################################################################################

	// Button callbacks

	void testCode();

	void uiDebugInfo();
	void serviceDebugInfo();

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
	bool getFeatureEnabled(const DebugServiceConfig::FeatureFlag& _flag) const { return m_config.getFeatureFlags().has(_flag); };
	
	void setFeatures(const DebugServiceConfig::Features& _features) { m_config.setFeatureFlags(_features); };
	const DebugServiceConfig::Features& getFeatures(void) const { return m_config.getFeatureFlags(); };

	// ###########################################################################################################################################################################################################################################################################################################################
	
	// ###########################################################################################################################################################################################################################################################################################################################
	
	// ###########################################################################################################################################################################################################################################################################################################################

	// Default

	static Application& instance();

private:
	virtual ~Application();

	struct ButtonInfo {
		ButtonInfo(ot::ToolBarButtonCfg&& _cfg, std::function<void(void)>&& _callback) 
			: cfg(std::move(_cfg)), callback(std::move(_callback)) {}
		ot::ToolBarButtonCfg cfg;
		std::function<void(void)> callback;
	};

	std::list<ButtonInfo> m_testButtons;

	std::string getButtonKey(const ButtonInfo& _info) const;

public:

	// ##################################################################################################################################################################################################################

	// Required functions

	virtual void initialize() override;

	//! @brief Will be called when the service was successfully started, the hppt and/or websocket connection is established and the service may start its work
	virtual void run(void) override;

	//! @brief Will be called when a UI connected to the session and is ready to work
	virtual void uiConnected(ot::components::UiComponent * _ui) override;

	//! @brief Will be called before the whole session starts to shut down (shut down can not be stopped)
	//! At this point all services, that are listed as connected, are still reachable.
	virtual void preShutdown(void) override;
};
