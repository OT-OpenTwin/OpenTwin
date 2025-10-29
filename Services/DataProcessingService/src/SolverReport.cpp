// @otlicense

#include "SolverReport.h"
#include "EntityResultText.h"

#include "Application.h"
#include "NewModelStateInfo.h"

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
			solverReport.setText(m_reportContent);
			solverReport.setName(m_solverName + " /Report");
			solverReport.storeToDataBase();

			ot::NewModelStateInfo modelStateInfo;
			modelStateInfo.addTopologyEntity(solverReport);
			ot::UID dataUID = solverReport.getTextDataStorageId();
			ot::UID dataVersion = solverReport.getTextDataStorageVersion();

			modelStateInfo.addDataEntity(solverReport.getEntityID(), dataUID, dataVersion);
			reportCreated = true;
		}	
	}

	return reportCreated;
}

