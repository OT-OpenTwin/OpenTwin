//! @file EntityHierarchicalScene.h
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

#include "EntityContainer.h"
#include "IVisualisationGraphicsView.h"

class OT_MODELENTITIES_API_EXPORT EntityHierarchicalScene : public EntityContainer, public IVisualisationGraphicsView {
public:
	EntityHierarchicalScene() : EntityHierarchicalScene(0, nullptr, nullptr, nullptr, "") {};
	EntityHierarchicalScene(ot::UID _ID, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms, const std::string& _owner);

	static std::string getClassTypeName() { return "EntityHierarchicalScene"; };
	virtual std::string getClassName(void) override { return EntityHierarchicalScene::getClassTypeName(); };

	ot::GraphicsNewEditorPackage* getGraphicsEditorPackage() override;
	bool visualiseGraphicsView() override { return true; };
	void addVisualizationNodes() override;
};
