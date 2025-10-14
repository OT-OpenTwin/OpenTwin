//! @file EntityHierarchicalScene.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

#include "EntityHierarchicalScene.h"
#include "OTGui/VisualisationTypes.h"
#include "OTCommunication/ActionTypes.h"

static EntityFactoryRegistrar<EntityHierarchicalScene> registrar(EntityHierarchicalScene::className());

EntityHierarchicalScene::EntityHierarchicalScene(ot::UID _ID, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms, const std::string& _owner) 
	: EntityContainer(_ID, _parent, _obs, _ms, _owner)
{
	OldTreeIcon treeIcons;
	treeIcons.size = 32;
	treeIcons.visibleIcon = "Hierarchical";
	treeIcons.hiddenIcon = "Hierarchical";
	this->setTreeIcon(treeIcons);

	this->setName(EntityHierarchicalScene::defaultName());
}

ot::GraphicsNewEditorPackage* EntityHierarchicalScene::getGraphicsEditorPackage() {
	ot::GraphicsNewEditorPackage* package = new ot::GraphicsNewEditorPackage(this->getName(), this->getName());
	return package;
}
