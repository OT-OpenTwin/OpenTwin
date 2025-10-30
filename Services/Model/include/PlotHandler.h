// @otlicense
// File: PlotHandler.h
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

#include "OTServiceFoundation/UiComponent.h"
#include "SelectionChangedObserver.h"
#include "EntityMetadataSeries.h"
#include "EntityResult1DPlot.h"
#include "OTGui/Plot1DCurveCfg.h"
#include "OTGuiAPI/ButtonHandler.h"
#include "NewModelStateInfo.h"

class PlotHandler : public SelectionChangedObserver
{
public:
	PlotHandler();

	void addButtons(ot::components::UiComponent* _uiComponent);

private:
	ot::ToolBarButtonCfg m_btnCreatePlot;
	ot::ToolBarButtonCfg m_btnAddCurveToPlot;
	
	const std::string c_groupName = "Plots";

	ot::ButtonHandler m_buttonHandler;
	void handleCreatePlot();
	void handleAddCurveToPlot();

	void updatedSelection(std::list<EntityBase*>& _selectedEntities, std::list<std::string>& _enabledButtons, std::list<std::string>& _disabledButtons) override;
	std::list<EntityMetadataSeries*> getSelectedSeriesMetadata();
	std::list<EntityResult1DPlot*> getSelectedPlots();
	std::string getFreePlotName();

	void createCurves(std::list<EntityMetadataSeries*>& _seriesMetadata, ot::NewModelStateInfo& _modelStateInformation, const std::string& _nameBase);
};
