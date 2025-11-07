// @otlicense
// File: SolverReport.cpp
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

#include "SolverReport.h"
#include "EntityResultText.h"

#include "Application.h"
#include "NewModelStateInfo.h"
#include "OTModelAPI/ModelServiceAPI.h"
SolverReport::~SolverReport()
{
	storeReport();
}

bool SolverReport::storeReport()
{
	bool reportCreated = false;
	if (m_solverName.empty())
	{
		assert(false);
	}
	else
	{
		if (!m_reportContent.empty())
		{
			EntityResultText solverReport;
			solverReport.setEntityID(Application::instance()->getModelComponent()->createEntityUID());
			solverReport.registerCallbacks(
				ot::EntityCallbackBase::Callback::Properties |
				ot::EntityCallbackBase::Callback::Selection,
				Application::instance()->getServiceName()
			);
			solverReport.setText(m_reportContent);
			solverReport.setName(m_solverName + "/Report");
			solverReport.storeToDataBase();

			ot::NewModelStateInfo modelStateInfo;
			modelStateInfo.addTopologyEntity(solverReport);
			ot::UID dataUID = solverReport.getTextDataStorageId();
			ot::UID dataVersion = solverReport.getTextDataStorageVersion();

			modelStateInfo.addDataEntity(solverReport.getEntityID(), dataUID, dataVersion);
			ot::ModelServiceAPI::addEntitiesToModel(modelStateInfo, "Added solver report");
			reportCreated = true;
		}	
	}

	return reportCreated;
}

