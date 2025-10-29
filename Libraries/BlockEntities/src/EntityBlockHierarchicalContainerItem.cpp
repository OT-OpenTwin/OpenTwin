// @otlicense

//! @file EntityBlockHierarchicalContainerItem.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "EntityFile.h"
#include "PropertyHelper.h"
#include "EntityProperties.h"
#include "EntityBlockHierarchicalContainerItem.h"

#include "OTGui/GraphicsHierarchicalProjectItemBuilder.h"

static EntityFactoryRegistrar<EntityBlockHierarchicalContainerItem> registrar(EntityBlockHierarchicalContainerItem::className());

EntityBlockHierarchicalContainerItem::EntityBlockHierarchicalContainerItem(ot::UID _ID, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms, const std::string& _owner)
	: EntityBlock(_ID, _parent, _obs, _ms, _owner) {
	OldTreeIcon icon;
	icon.visibleIcon = "Hierarchical/Container";
	icon.hiddenIcon = "Hierarchical/Container";
	setNavigationTreeIcon(icon);

	setBlockTitle("Container");

	resetModified();
}

ot::GraphicsItemCfg* EntityBlockHierarchicalContainerItem::createBlockCfg() {
	ot::GraphicsHierarchicalProjectItemBuilder builder;

	// Mandatory settings
	builder.setName(this->getName());
	builder.setTitle(this->createBlockHeadline());
	builder.setLeftTitleCornerImagePath("Hierarchical/Container.png");
	builder.setTitleBackgroundGradientColor(ot::Green);
	builder.setPreviewImagePath("Hierarchical/ContainerBackground.png");

	// Create the item
	return builder.createGraphicsItem();
}
