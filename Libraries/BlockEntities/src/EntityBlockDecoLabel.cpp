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
	ot::EntityTreeItem treeItem = getTreeItem();
	treeItem.setVisibleIcon("Tree/Image");
	treeItem.setHiddenIcon("Tree/Image");
	setDefaultTreeItem(treeItem);

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
	cfg->setTextFont(getFont());
	cfg->setAlignment(getAlignment());

	this->applyDecorationPropertiesToCfg(cfg.get());

	return cfg.release();
}

bool EntityBlockDecoLabel::updateFromProperties() {
	return EntityBlockDecoration::updateFromProperties();
}

void EntityBlockDecoLabel::createProperties() {
	EntityBlockDecoration::createProperties();
	
	EntityPropertiesString* textProp = EntityPropertiesString::createProperty("Label", "Text", "New Label", "", getProperties());
	textProp->setIsMultiline(true);

	EntityPropertiesGuiPainter* textColorProp = EntityPropertiesGuiPainter::createProperty("Label", "Text Color", 
		new ot::StyleRefPainter2D(ot::ColorStyleValueEntry::GraphicsItemForeground), "", getProperties());

	EntityPropertiesSelection* fontFamilyProp = EntityPropertiesSelection::createProperty("Label", "Font Family", ot::getAllFontFamilyStringList(), ot::toString(ot::FontFamily::Consolas), "", getProperties());
	EntityPropertiesInteger* fontSizeProp = EntityPropertiesInteger::createProperty("Label", "Font Size", 12, 1, 1000, "", getProperties());
	fontSizeProp->setSuffix("px");
	fontSizeProp->setAllowCustomValues(false);

	EntityPropertiesBoolean* boldProp = EntityPropertiesBoolean::createProperty("Label", "Bold", false, "", getProperties());
	EntityPropertiesBoolean* italicProp = EntityPropertiesBoolean::createProperty("Label", "Italic", false, "", getProperties());

	EntityPropertiesSelection* alignProp = EntityPropertiesSelection::createProperty("Label", "Alignment", 
		{ ot::toString(ot::Alignment::Center), ot::toString(ot::Alignment::Top), ot::toString(ot::Alignment::TopRight), ot::toString(ot::Alignment::Right), 
		  ot::toString(ot::Alignment::BottomRight), ot::toString(ot::Alignment::Bottom), ot::toString(ot::Alignment::BottomLeft), 
		  ot::toString(ot::Alignment::Left), ot::toString(ot::Alignment::TopLeft) },
		ot::toString(ot::Alignment::Center), "", getProperties());
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

void EntityBlockDecoLabel::setFontFamily(ot::FontFamily _fontFamily) {
	PropertyHelper::setSelectionPropertyValue(ot::toString(_fontFamily), this, "Font Family", "Label");
}

void EntityBlockDecoLabel::setFontFamily(const std::string& _fontFamily) {
	PropertyHelper::setSelectionPropertyValue(_fontFamily, this, "Font Family", "Label");
}

std::string EntityBlockDecoLabel::getFontFamily() const {
	return PropertyHelper::getSelectionPropertyValue(this, "Font Family", "Label");
}

void EntityBlockDecoLabel::setFontSize(int _fontSize) {
	PropertyHelper::setIntegerPropertyValue(_fontSize, this, "Font Size", "Label");
}

int EntityBlockDecoLabel::getFontSize() const {
	return PropertyHelper::getIntegerPropertyValue(this, "Font Size", "Label");
}

void EntityBlockDecoLabel::setBold(bool _bold) {
	PropertyHelper::setBoolPropertyValue(_bold, this, "Bold", "Label");
}

bool EntityBlockDecoLabel::getBold() const {
	return PropertyHelper::getBoolPropertyValue(this, "Bold", "Label");
}

void EntityBlockDecoLabel::setItalic(bool _italic) {
	PropertyHelper::setBoolPropertyValue(_italic, this, "Italic", "Label");
}

bool EntityBlockDecoLabel::getItalic() const {
	return PropertyHelper::getBoolPropertyValue(this, "Italic", "Label");
}

void EntityBlockDecoLabel::setFont(const ot::Font& _font) {
	PropertyHelper::setSelectionPropertyValue(_font.family(), this, "Font Family", "Label");
	PropertyHelper::setIntegerPropertyValue(_font.size(), this, "Font Size", "Label");
	PropertyHelper::setBoolPropertyValue(_font.isBold(), this, "Bold", "Label");
	PropertyHelper::setBoolPropertyValue(_font.isItalic(), this, "Italic", "Label");
}

ot::Font EntityBlockDecoLabel::getFont() const {
	ot::Font font;
	
	font.setFamily(PropertyHelper::getSelectionPropertyValue(this, "Font Family", "Label"));
	font.setSize(PropertyHelper::getIntegerPropertyValue(this, "Font Size", "Label"));
	font.setBold(PropertyHelper::getBoolPropertyValue(this, "Bold", "Label"));
	font.setItalic(PropertyHelper::getBoolPropertyValue(this, "Italic", "Label"));

	return font;
}

void EntityBlockDecoLabel::setAlignment(ot::Alignment _alignment) {
	PropertyHelper::setSelectionPropertyValue(ot::toString(_alignment), this, "Alignment", "Label");
}

ot::Alignment EntityBlockDecoLabel::getAlignment() const {
	return ot::stringToAlignment(PropertyHelper::getSelectionPropertyValue(this, "Alignment", "Label"));
}

// ###########################################################################################################################################################################################################################################################################################################################

// Protected

void EntityBlockDecoLabel::addStorageData(bsoncxx::builder::basic::document& _storage) {
	EntityBlockDecoration::addStorageData(_storage);
}

void EntityBlockDecoLabel::readSpecificDataFromDataBase(const bsoncxx::document::view& _docView, std::map<ot::UID, EntityBase*>& _entityMap) {
	EntityBlockDecoration::readSpecificDataFromDataBase(_docView, _entityMap);
}
