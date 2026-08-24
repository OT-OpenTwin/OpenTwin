// @otlicense
// File: ProjectInformationHandler.h
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

// OpenTwin header
#include "OTCore/ProjectInformation.h"
#include "OTCore/ProjectCompareConfig.h"
#include "OTCommunication/Handler/ActionHandler.h"
#include "OTGuiAPI/ButtonHandler.h"
#include "OTModelEntities/RAII/ParallelCollectionRAII.h"
#include "OTServiceFoundation/UiComponent.h"

// std header
#include <map>
#include <optional>

class ModelState;
class ProgressUpdater;

class ProjectInformationHandler : public ot::ActionHandler, public ot::ButtonHandler {
	OT_DECL_NOCOPY(ProjectInformationHandler)
	OT_DECL_NOMOVE(ProjectInformationHandler)
public:
	ProjectInformationHandler(ot::ActionDispatcher* _dispatcher = &ot::ActionDispatcher::instance());
	~ProjectInformationHandler();

	void addButtons(ot::components::UiComponent* _ui);

	std::optional<std::string> getCollectionName(const std::string& _projectName);

private:

	// ##################################################################################################################################################################################################################

	// Action handler

	ot::ReturnMessage applyProjectInformation(ot::JsonDocument& _document);
	ot::ReturnMessage getProjectVersionGraph(ot::JsonDocument& _document);
	ot::ReturnMessage startComparison(ot::JsonDocument& _document);

	// ##################################################################################################################################################################################################################

	// Button handler

	void requestUploadProjectPreviewImage();
	void requestProjectVersionSelectionForComparison();

	// ##################################################################################################################################################################################################################

	// Comparison

	void comparisonWorker(ot::ProjectCompareConfig&& _config);

	struct ComparisonData
	{
		enum ComparisonStep {
			InitialStep,
			StepOpenOtherProject,
			StepCompare,
			StepDone,
			StepCount
		};

		ComparisonData() = delete;
		ComparisonData(const ComparisonData&) = delete;
		ComparisonData(ComparisonData&&) = delete;
		ComparisonData(ot::ProjectCompareConfig&& _config, ot::ParallelCollectionRAII&& _collectionSwitch, ModelState* _leftState, ModelState* _rightState)
			: config(std::move(_config)), collectionSwitch(std::move(_collectionSwitch)), leftState(_leftState), rightState(_rightState)
		{};

		void initializeUpdater(ProgressUpdater* _updater);
		void switchToStep(ComparisonStep _step);

		ot::ProjectCompareConfig config;
		ot::ParallelCollectionRAII collectionSwitch;
		ModelState* leftState;
		ModelState* rightState;

		ComparisonStep step = ComparisonStep::InitialStep;
		ProgressUpdater* progressUpdater = nullptr;
	};

	void comparisonStepEntities(ComparisonData& _data);
	void comparisonStepDone(ComparisonData& _data);

	// ##################################################################################################################################################################################################################

	// Helper

	void requestProjectInformation(const std::string& _projectName);
	void loadAuthorisationURL();

	ot::ToolBarButtonCfg m_editInfoButton;
	ot::ToolBarButtonCfg m_compareToButton;
	
	std::atomic_bool m_isComparisonRunning{ false };
	const std::string m_comparisonVersionSelectionResponseAction;

	std::map<std::string, ot::ProjectInformation> m_projectInfoCacheByName;
};