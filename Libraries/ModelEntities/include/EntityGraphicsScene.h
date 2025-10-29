// @otlicense

#pragma once
#include "EntityContainer.h"
#include "IVisualisationGraphicsView.h"

class __declspec(dllexport) EntityGraphicsScene : public EntityContainer, public IVisualisationGraphicsView
{
public:
	EntityGraphicsScene() : EntityGraphicsScene(0, nullptr, nullptr, nullptr, "") {};
	EntityGraphicsScene(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, const std::string& owner);
	static std::string className() { return "EntityGraphicsScene"; };
	virtual std::string getClassName(void) const override { return EntityGraphicsScene::className(); };
	ot::GraphicsNewEditorPackage* getGraphicsEditorPackage() override;
	bool visualiseGraphicsView() override;
	void addVisualizationNodes() override;
};
