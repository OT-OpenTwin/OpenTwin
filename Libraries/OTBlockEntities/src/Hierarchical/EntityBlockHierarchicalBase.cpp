// @otlicense
// File: EntityBlockHierarchicalBase.cpp
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
#include "OTGui/Graphics/Builder/GraphicsHierarchicalItemBuilder.h"
#include "OTGui/Painter/StyleRefPainter2D.h"
#include "OTModelEntities/EntityBinaryData.h"
#include "OTModelEntities/Properties/PropertyHelper.h"
#include "OTModelEntities/Properties/EntityPropertiesItems.h"
#include "OTBlockEntities/Hierarchical/EntityBlockHierarchicalBase.h"

ot::EntityBlockHierarchicalBase::EntityBlockHierarchicalBase(UID _ID, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms)
	: EntityBlock(_ID, _parent, _obs, _ms), m_centerImageUID(ot::invalidUID), m_centerImageVersion(ot::invalidUID), m_centerImageData(nullptr),
	m_centerImageFormat(ot::ImageFileFormat::PNG)
{}

bool ot::EntityBlockHierarchicalBase::updateFromProperties() {
	bool needsUpdate = EntityBlock::updateFromProperties();

	bool useCustomSize = getUseCustomSize();

	EntityPropertiesBase* sizeProp = PropertyHelper::getPropertyBase(this, "Width", std::string(generalPropertyGroupName));
	if (sizeProp->getVisible() != useCustomSize)
	{
		sizeProp->setVisible(useCustomSize);
		PropertyHelper::getPropertyBase(this, "Height", std::string(generalPropertyGroupName))->setVisible(useCustomSize);
		needsUpdate = true;
	}

	getProperties().forceResetUpdateForAllProperties();

	createBlockItem();

	return needsUpdate;
}

void ot::EntityBlockHierarchicalBase::createProperties()
{
	EntityBlock::createProperties();

	const std::string generalGroup = std::string(this->generalPropertyGroupName);
	const std::string titleGroup = std::string(this->titlePropertyGroupName);
	const std::string centerImageGroup = std::string(this->centerImagePropertyGroupName);
	const std::string footerGroup = std::string(this->footerPropertyGroupName);

	EntityPropertiesBase* prop = EntityPropertiesBoolean::createProperty(generalGroup, "Custom Size", false, "", getProperties());

	EntityPropertiesInteger* intProp = EntityPropertiesInteger::createProperty(generalGroup, "Width", 100, 1, 10000, "", getProperties());
	intProp->setAllowCustomValues(false);
	intProp->setVisible(false);

	intProp = EntityPropertiesInteger::createProperty(generalGroup, "Height", 100, 1, 10000, "", getProperties());
	intProp->setAllowCustomValues(false);
	intProp->setVisible(false);

	createTextProperties(titleGroup, true);
	createImageProperties(centerImageGroup, true);
	createTextProperties(footerGroup, false);

}

void ot::EntityBlockHierarchicalBase::fillContextMenu(const MenuRequestData* _requestData, MenuCfg& _menu)
{
	_menu.addButton("Open", "Open", "ContextMenu/Open.png", MenuButtonCfg::ButtonAction::TriggerButton)
		->setTriggerButton("Project/Selection/Open");

	EntityBlock::fillContextMenu(_requestData, _menu);
}

ot::GraphicsItemCfg* ot::EntityBlockHierarchicalBase::createBlockCfg()
{
	ensureCenterImageLoaded();

	ot::GraphicsHierarchicalItemBuilder builder;

	// General
	if (getUseCustomSize())
	{
		builder.setFixedSize(getCustomSize());
	}

	builder.setEntityName(this->getName());

	// Title
	std::string titleText = this->getCustomTitle();
	if (titleText.empty())
	{
		titleText = this->createBlockHeadline();
	}

	builder.setTopText(titleText);
	builder.setTopTextFont(this->getTitleFont());
	builder.setTopTextPainter(this->getTitlePainter()->createCopy());
	builder.setTopTextAlignment(this->getTitleAlignment());

	// Image
	if (hasCenterImage())
	{
		builder.setCenterImageData(m_centerImageData->getData(), m_centerImageFormat);
		builder.setCenterImageAlignment(getCenterImageAlignment());
		builder.setCenterImageMaintainAspectRatio(getMaintainCenterImageAspectRatio());
		builder.setCenterImageFixedSize(getCenterImageSize());
	}

	// Footer
	builder.setBottomText(getFooterText());
	builder.setBottomTextFont(this->getFooterFont());
	builder.setBottomTextPainter(this->getFooterPainter()->createCopy());
	builder.setBottomTextAlignment(getFooterAlignment());

	// Create the item
	return builder.createGraphicsItem();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Data accessors

void ot::EntityBlockHierarchicalBase::setCenterImage(ot::UID _entityID, ot::UID _entityVersion, ot::ImageFileFormat _format)
{
	m_centerImageUID = _entityID;
	m_centerImageVersion = _entityVersion;
	m_centerImageFormat = _format;
	m_centerImageData.reset();
	m_centerImageData = nullptr;

	setModified();
}

void ot::EntityBlockHierarchicalBase::removeCenterImage()
{
	m_centerImageUID = ot::invalidUID;
	m_centerImageVersion = ot::invalidUID;
	m_centerImageData.reset();
	m_centerImageData = nullptr;

	setModified();
}

std::shared_ptr<EntityBinaryData> ot::EntityBlockHierarchicalBase::getCenterImageData()
{
	ensureCenterImageLoaded();
	return m_centerImageData;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Property accessors

bool ot::EntityBlockHierarchicalBase::getUseCustomSize() const
{
	return PropertyHelper::getBoolPropertyValue(this, "Custom Size", std::string(generalPropertyGroupName));
}

int ot::EntityBlockHierarchicalBase::getCustomWidth() const
{
	return PropertyHelper::getIntegerPropertyValue(this, "Width", std::string(generalPropertyGroupName));
}

int ot::EntityBlockHierarchicalBase::getCustomHeight() const
{
	return PropertyHelper::getIntegerPropertyValue(this, "Height", std::string(generalPropertyGroupName));
}

ot::Size2DD ot::EntityBlockHierarchicalBase::getCustomSize() const
{
	return Size2DD(static_cast<double>(getCustomWidth()), static_cast<double>(getCustomHeight()));
}

std::string ot::EntityBlockHierarchicalBase::getCustomTitle() const
{
	return PropertyHelper::getStringPropertyValue(this, "Text", std::string(titlePropertyGroupName));
}

ot::Font ot::EntityBlockHierarchicalBase::getTitleFont() const
{
	Font font;

	font.setFamily(PropertyHelper::getSelectionPropertyValue(this, "Font Family", std::string(titlePropertyGroupName)));
	font.setSize(PropertyHelper::getIntegerPropertyValue(this, "Font Size", std::string(titlePropertyGroupName)));
	font.setBold(PropertyHelper::getBoolPropertyValue(this, "Bold", std::string(titlePropertyGroupName)));
	font.setItalic(PropertyHelper::getBoolPropertyValue(this, "Italic", std::string(titlePropertyGroupName)));

	return font;
}

const ot::Painter2D* ot::EntityBlockHierarchicalBase::getTitlePainter() const
{
	return PropertyHelper::getPainterPropertyValue(this, "Text Color", std::string(titlePropertyGroupName));
}

ot::Alignment ot::EntityBlockHierarchicalBase::getTitleAlignment() const
{
	return stringToAlignment(PropertyHelper::getSelectionPropertyValue(this, "Alignment", std::string(titlePropertyGroupName)));
}

bool ot::EntityBlockHierarchicalBase::getMaintainCenterImageAspectRatio() const
{
	return PropertyHelper::getBoolPropertyValue(this, "Maintain Aspect Ratio", std::string(centerImagePropertyGroupName));
}

ot::Alignment ot::EntityBlockHierarchicalBase::getCenterImageAlignment() const
{
	return stringToAlignment(PropertyHelper::getSelectionPropertyValue(this, "Alignment", std::string(centerImagePropertyGroupName)));
}

int ot::EntityBlockHierarchicalBase::getCenterImageWidth() const
{
	return PropertyHelper::getIntegerPropertyValue(this, "Width", std::string(centerImagePropertyGroupName));
}

int ot::EntityBlockHierarchicalBase::getCenterImageHeight() const
{
	return PropertyHelper::getIntegerPropertyValue(this, "Height", std::string(centerImagePropertyGroupName));
}

ot::Size2DD ot::EntityBlockHierarchicalBase::getCenterImageSize() const
{
	return Size2DD(static_cast<double>(getCenterImageWidth()), static_cast<double>(getCenterImageHeight()));
}

std::string ot::EntityBlockHierarchicalBase::getFooterText() const
{
	return PropertyHelper::getStringPropertyValue(this, "Text", std::string(footerPropertyGroupName));
}

ot::Font ot::EntityBlockHierarchicalBase::getFooterFont() const
{
	Font font;

	font.setFamily(PropertyHelper::getSelectionPropertyValue(this, "Font Family", std::string(footerPropertyGroupName)));
	font.setSize(PropertyHelper::getIntegerPropertyValue(this, "Font Size", std::string(footerPropertyGroupName)));
	font.setBold(PropertyHelper::getBoolPropertyValue(this, "Bold", std::string(footerPropertyGroupName)));
	font.setItalic(PropertyHelper::getBoolPropertyValue(this, "Italic", std::string(footerPropertyGroupName)));

	return font;
}

const ot::Painter2D* ot::EntityBlockHierarchicalBase::getFooterPainter() const
{
	return PropertyHelper::getPainterPropertyValue(this, "Text Color", std::string(footerPropertyGroupName));
}

ot::Alignment ot::EntityBlockHierarchicalBase::getFooterAlignment() const
{
	return stringToAlignment(PropertyHelper::getSelectionPropertyValue(this, "Alignment", std::string(footerPropertyGroupName)));
}

// ###########################################################################################################################################################################################################################################################################################################################

// Protected

void ot::EntityBlockHierarchicalBase::createNavigationTreeEntry()
{
	EntityBlock::createNavigationTreeEntry();
}

void ot::EntityBlockHierarchicalBase::addStorageData(bsoncxx::builder::basic::document& _storage)
{
	EntityBlock::addStorageData(_storage);

	_storage.append(
		bsoncxx::builder::basic::kvp("CenterImageID", static_cast<int64_t>(m_centerImageUID)),
		bsoncxx::builder::basic::kvp("CenterImageVersion", static_cast<int64_t>(m_centerImageVersion)),
		bsoncxx::builder::basic::kvp("CenterImageType", ot::toString(m_centerImageFormat))
	);
}

void ot::EntityBlockHierarchicalBase::readSpecificDataFromDataBase(const bsoncxx::document::view& _docView, std::map<ot::UID, EntityBase*>& _entityMap)
{
	EntityBlock::readSpecificDataFromDataBase(_docView, _entityMap);

	auto docIt = _docView.find("CenterImageID");
	if (docIt != _docView.end())
	{
		m_centerImageUID = static_cast<ot::UID>(docIt->get_int64());
		m_centerImageVersion = static_cast<ot::UID>(_docView["CenterImageVersion"].get_int64());
		m_centerImageFormat = ot::stringToImageFileFormat(_docView["CenterImageType"].get_string().value.data());
	}
}

void ot::EntityBlockHierarchicalBase::ensureCenterImageLoaded()
{
	if (m_centerImageUID == ot::invalidUID)
	{
		return;
	}
	if (m_centerImageData != nullptr)
	{
		return;
	}

	std::map<ot::UID, EntityBase*> entityMap;
	m_centerImageData.reset(dynamic_cast<EntityBinaryData*>(readEntityFromEntityIDAndVersion(this, m_centerImageUID, m_centerImageVersion, entityMap)));
}

void ot::EntityBlockHierarchicalBase::createTextProperties(const std::string& _group, bool _isTopText)
{
	EntityPropertiesString* sprop = EntityPropertiesString::createProperty(_group, "Text", "", "", getProperties());
	if (_isTopText)
	{
		sprop->setToolTip("Text to display. If empty the short entity name will be displayed instead.");
	}
	
	EntityPropertiesBase* prop = EntityPropertiesSelection::createProperty(_group, "Font Family", getAllFontFamilyStringList(), toString(FontFamily::Consolas), "", getProperties());

	EntityPropertiesInteger* fontSizeProp = EntityPropertiesInteger::createProperty(_group, "Font Size", 12, 1, 1000, "", getProperties());
	fontSizeProp->setAllowCustomValues(false);

	prop = EntityPropertiesBoolean::createProperty(_group, "Bold", false, "", getProperties());
	prop = EntityPropertiesBoolean::createProperty(_group, "Italic", false, "", getProperties());
	prop = EntityPropertiesGuiPainter::createProperty(_group, "Text Color", new StyleRefPainter2D(ColorStyleValueEntry::GraphicsItemForeground), "", getProperties());
	prop = EntityPropertiesSelection::createProperty(_group, "Alignment", getAllAlignmentStringList(), toString(Alignment::Center), "", getProperties());
}

void ot::EntityBlockHierarchicalBase::createImageProperties(const std::string& _group, bool _isCenter)
{
	EntityPropertiesBase* prop = EntityPropertiesBoolean::createProperty(_group, "Maintain Aspect Ratio", true, "", getProperties());
	prop = EntityPropertiesSelection::createProperty(_group, "Alignment", getAllAlignmentStringList(), toString(Alignment::Center), "", getProperties());

	EntityPropertiesInteger* intProp = EntityPropertiesInteger::createProperty(_group, "Width", 150, 1, 10000, "", getProperties());
	intProp->setAllowCustomValues(false);

	intProp = EntityPropertiesInteger::createProperty(_group, "Height", 150, 1, 10000, "", getProperties());
	intProp->setAllowCustomValues(false);
}
