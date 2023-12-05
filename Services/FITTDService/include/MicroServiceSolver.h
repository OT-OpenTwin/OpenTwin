#pragma once

#include "OTServiceFoundation/ModelComponent.h"
#include "OTServiceFoundation/UiComponent.h"

#include "ModelState.h"
#include "ClassFactory.h"

#include "EntityMeshCartesianData.h"
#include "EntityUnits.h"
#include "SolverSettings/SolverSettings.h"
#include "SolverSettings/PortSettings.h"
#include "SolverSettings/MonitorSettings.h"

class MicroServiceSolver
{
public:
	MicroServiceSolver(std::string solverName, int serviceID, int sessionCount);
	virtual ~MicroServiceSolver();

	void setDataBaseURL(const std::string &dbURL);
	void setProjectName(const std::string &prjName);
	void setUIComponent(ot::components::UiComponent *uiComp);
	void setModelComponent(ot::components::ModelComponent *mdlComp);
	void setModelStateVersion(const std::string &modelVersion);
	void setSolverEntityID(ot::UID entityID);

	void setMeshDataEntityID(ot::UID entityID);
	void setMeshDataEntityVersionID(ot::UID entityID);

	void setSolverTopologyBase(const std::string &base);

	void displayMessage(const std::string &msg);

	void setProgressInformation(std::string message, bool continuous);
	void setProgress(int percentage);
	void closeProgressInformation(void);

	void RemoveOldResults(void);

	std::pair<ot::UID, std::string> addResultCurve(const std::string &name,
									           std::vector<double> xdata, std::vector<double> ydataRe, std::vector<double> ydataIm,
									           std::string xlabel, std::string xunit, std::string ylabel, std::string yunit, int colorID, bool visualize);
	void addPlot1D(std::string name, std::string title, const std::list<std::pair<ot::UID, std::string>> &curves);
	void addResultFD(std::string name, std::string title, double * xComponent, double * yComponent, double * zComponent, size_t size);

	void StoreLogDataInResultItem(std::string logData);

	std::list<ot::UID> getResultTopologyIDList(void);
	std::list<ot::UID> getResultTopologyVersionList(void);
	std::list<bool> getResultForceVisibleList(void);
	std::list<ot::UID> getResultDataIDList(void);
	std::list<ot::UID> getResultDataVersionList(void);
	std::list<ot::UID> getResultDataParentList(void);

protected:
	ot::components::UiComponent *uiComponent;
	ot::components::ModelComponent *modelComponent;
	ot::UID meshDataEntityID;
	ot::UID meshDataVersion;
	std::string solverName;

	EntityBase * LoadEntityFromName(std::string name);
	
	SolverSettings LoadSolverSettings(std::string solverName);
	EntityMeshCartesianData * LoadMeshData(std::string meshName);
	Signal LoadSignal(std::string signalName);
	PortSettings LoadPortSetting(std::string portName, std::string solverName);
	MonitorSettings LoadMonitorSetting(std::string monitorName);

	void ApplyFrequencyUnit(double& value, std::string& formerUnit);
	void UpdateModel(void);
private:
	std::string dataBaseURL;
	std::string projectName;
	std::string modelStateVersion;
	ot::UID solverEntityID;

	std::list<ot::UID> topologyEntityIDList;
	std::list<ot::UID> topologyEntityVersionList;
	std::list<bool> topologyEntityForceVisibleList;
	std::list<ot::UID> dataEntityIDList;
	std::list<ot::UID> dataEntityVersionList;
	std::list<ot::UID> dataEntityParentList;

	ClassFactory factory;
	EntityUnits * _units = nullptr;
};


