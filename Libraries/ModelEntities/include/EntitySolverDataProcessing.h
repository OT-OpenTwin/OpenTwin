#pragma once
#include "EntitySolver.h"

class __declspec(dllexport) EntitySolverDataProcessing : public EntitySolver
{
public:
	EntitySolverDataProcessing() : EntitySolverDataProcessing(0, nullptr, nullptr, nullptr, "") {};
	EntitySolverDataProcessing(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, const std::string& owner)
		: EntitySolver(ID,parent,obs,ms,owner){ }
	virtual ~EntitySolverDataProcessing() = default;
	virtual std::string getClassName(void) override { return "EntitySolverDataProcessing"; };

	void createProperties(const std::string& _graphicsSceneFolder, const ot::UID _graphicsSceneFolderID);

	const std::string getSelectedPipeline() ;

};
