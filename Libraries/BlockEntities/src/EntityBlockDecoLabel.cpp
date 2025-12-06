// @otlicense
// File: EntityBlockDecoLabel.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

// OpenTwin header
#include "EntityFile.h"
#include "PropertyHelper.h"
#include "EntityProperties.h"
#include "EntityBlockDecoLabel.h"
#include "OTGui/StyleRefPainter2D.h"
#include "OTGui/GraphicsTextItemCfg.h"

static EntityFactoryRegistrar<EntityBlockDecoLabel> registrar(EntityBlockDecoLabel::className());

EntityBlockDecoLabel::EntityBlockDecoLabel(ot::UID _ID, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms)
	: EntityBlockDecoration(_ID, _parent, _obs, _ms)
{
	OldTreeIcon icon;
	icon.visibleIcon = "Tree/Image";
	icon.hiddenIcon = "Tree/Image";
	setNavigationTreeIcon(icon);

	setBlockTitle("Image");

	resetModified();
}

ot::GraphicsItemCfg* EntityBlockDecoLabel::createBlockCfg() {
	std::unique_ptr<ot::GraphicsTextItemCfg> cfg(new ot::GraphicsTextItemCfg);

	ot::GraphicsItemCfg::GraphicsItemFlags itemFlags = ot::GraphicsItemCfg::ItemIsSelectable | ot::GraphicsItemCfg::ItemSnapsToGridTopLeft |
		ot::GraphicsItemCfg::ItemHandlesState | ot::GraphicsItemCfg::ItemUserTransformEnabled;

	cfg->setGraphicsItemFlags(itemFlags);
	cfg->setText(getText());
	cfg->setTextPainter(getTextPainter()->createCopy());

	this->applyDecorationPropertiesToCfg(cfg.get());

	return cfg.release();
}

bool EntityBlockDecoLabel::updateFromProperties() {
	return EntityBlockDecoration::updateFromProperties();
}

void EntityBlockDecoLabel::createProperties() {
	EntityBlockDecoration::createProperties();
	EntityPropertiesString* textProp = EntityPropertiesString::createProperty("Label", "Text", "New Label", "", getProperties());
	EntityPropertiesGuiPainter* textColorProp = EntityPropertiesGuiPainter::createProperty("Label", "Text Color", 
		new ot::StyleRefPainter2D(ot::ColorStyleValueEntry::GraphicsItemForeground), "", getProperties());
}

// ###########################################################################################################################################################################################################################################################################################################################

// Property accessors

void EntityBlockDecoLabel::setText(const std::string& _text) {
	PropertyHelper::setStringPropertyValue(_text, this, "Text", "Label");
}

std::string EntityBlockDecoLabel::getText() const {
	return PropertyHelper::getStringPropertyValue(this, "Text", "Label");
}

void EntityBlockDecoLabel::setTextPainter(const ot::Painter2D* _painter) {
	PropertyHelper::setPainterPropertyValue(_painter, this, "Text Color", "Label");
}

const ot::Painter2D* EntityBlockDecoLabel::getTextPainter() const {
	return PropertyHelper::getPainterPropertyValue(this, "Text Color", "Label");
}


// ###########################################################################################################################################################################################################################################################################################################################

// Protected

void EntityBlockDecoLabel::addStorageData(bsoncxx::builder::basic::document& _storage) {
	EntityBlockDecoration::addStorageData(_storage);
}

void EntityBlockDecoLabel::readSpecificDataFromDataBase(const bsoncxx::document::view& _docView, std::map<ot::UID, EntityBase*>& _entityMap) {
	EntityBlockDecoration::readSpecificDataFromDataBase(_docView, _entityMap);
}
