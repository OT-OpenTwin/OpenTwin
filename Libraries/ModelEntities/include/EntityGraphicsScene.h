#pragma once
#include "EntityContainer.h"
#include "IVisualisationGraphicsView.h"

class __declspec(dllexport) EntityGraphicsScene : public EntityContainer, public IVisualisationGraphicsView
{
public:
	EntityGraphicsScene() : EntityGraphicsScene(0, nullptr, nullptr, nullptr, "") {};
	EntityGraphicsScene(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, const std::string& owner);
	virtual std::string getClassName(void) override { return "EntityGraphicsScene"; };
	ot::GraphicsNewEditorPackage* getGraphicsEditorPackage() override;
	bool visualiseGraphicsView() override;
	void addVisualizationNodes() override;
};
