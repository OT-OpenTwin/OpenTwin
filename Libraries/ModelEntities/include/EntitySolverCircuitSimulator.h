#pragma once
#pragma warning(disable : 4251)

#include "EntitySolver.h"
#include "OldTreeIcon.h"

#include <list>

class __declspec(dllexport) EntitySolverCircuitSimulator : public EntitySolver
{
public:
	EntitySolverCircuitSimulator(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner);
	virtual ~EntitySolverCircuitSimulator();

	void createProperties(const std::string circuitFolderName, ot::UID circuitFolderID, const std::string circuitName, ot::UID circuitID);
	virtual bool updateFromProperties(void) override;

	virtual std::string getClassName(void) override { return "EntitySolverCircuitSimulator"; };
	
protected:
	virtual int getSchemaVersion(void) override { return 1; };

private:
	bool SetVisibleDCSimulationParameters(bool visible);
	bool SetVisibleTRANSimulationParameters(bool visible);
	bool SetVisibleACSimulationParameters(bool visible);
	void createDCProperties();
	void createTranProperties();
	void createACProperties();

};
