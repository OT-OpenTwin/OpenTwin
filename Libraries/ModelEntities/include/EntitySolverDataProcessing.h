#pragma once
#include "EntitySolver.h"
#include "IVisualisationGraphicsView.h"

class __declspec(dllexport) EntitySolverDataProcessing : public EntitySolver, public IVisualisationGraphicsView
{
public:
	EntitySolverDataProcessing(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner)
		: EntitySolver(ID,parent,obs,ms,factory,owner){ }
	virtual ~EntitySolverDataProcessing() = default;
	virtual void addVisualizationNodes(void) override;
	virtual std::string getClassName(void) override { return "EntitySolverDataProcessing"; };

	ot::GraphicsNewEditorPackage* getGraphicsEditorPackage() override;
	bool visualiseGraphicsView() override;
};
