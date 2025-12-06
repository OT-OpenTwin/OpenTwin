// @otlicense
// File: EntityBlockImage.cpp
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
#include "EntityBinaryData.h"
#include "EntityProperties.h"
#include "EntityBlockImage.h"
#include "OTGui/StyleRefPainter2D.h"
#include "OTGui/GraphicsImageItemCfg.h"

static EntityFactoryRegistrar<EntityBlockImage> registrar(EntityBlockImage::className());

EntityBlockImage::EntityBlockImage(ot::UID _ID, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms)
	: EntityBlock(_ID, _parent, _obs, _ms), m_imageUID(ot::invalidUID), m_imageVersion(ot::invalidUID),
	m_image(nullptr), m_imageFormat(ot::ImageFileFormat::PNG)
{
	ot::EntityTreeItem treeItem;
	treeItem.setVisibleIcon("Tree/Image");
	treeItem.setHiddenIcon("Tree/Image");
	this->setTreeItem(treeItem, true);

	setBlockTitle("Image");

	resetModified();
}

ot::GraphicsItemCfg* EntityBlockImage::createBlockCfg() {
	ensureFileIsLoaded();

	if (!m_image) {
		OT_LOG_E("Cannot create block image item without a valid image file assigned to the block image entity.");
		return nullptr;
	}

	std::unique_ptr<ot::GraphicsImageItemCfg> cfg(new ot::GraphicsImageItemCfg);

	ot::GraphicsItemCfg::GraphicsItemFlags itemFlags = ot::GraphicsItemCfg::ItemIsSelectable | ot::GraphicsItemCfg::ItemSnapsToGridTopLeft | ot::GraphicsItemCfg::ItemHandlesState;

	if (!PropertyHelper::getBoolPropertyValue(this, "Lock movement", "Block")) {
		itemFlags |= ot::GraphicsItemCfg::ItemIsMoveable;
	}

	cfg->setGraphicsItemFlags(itemFlags);
	cfg->setImageData(m_image->getData(), m_imageFormat);
	int width = PropertyHelper::getIntegerPropertyValue(this, "Width", "Image");
	int height = PropertyHelper::getIntegerPropertyValue(this, "Height", "Image");

	if (width > 0 && height > 0) {
		cfg->setFixedSize(width, height);
	}
	cfg->setMaintainAspectRatio(PropertyHelper::getBoolPropertyValue(this, "Maintain aspect ratio", "Image"));
	cfg->setZValue(PropertyHelper::getIntegerPropertyValue(this, "Z Value", "Block"));

	ot::PenFCfg borderPen(1., ot::DefaultColor::Transparent);
	if (PropertyHelper::getBoolPropertyValue(this, "Draw border", "Image")) {
		borderPen.setWidth(PropertyHelper::getDoublePropertyValue(this, "Border width", "Image"));
		ot::Painter2D* penPainter = PropertyHelper::getPainterProperty(this, "Border color", "Image")->getValue()->createCopy();
		borderPen.setPainter(penPainter);
	}
	cfg->setBorderPen(borderPen);

	return cfg.release();
}

bool EntityBlockImage::updateFromProperties() {
	bool updateGrid = false;

	const bool drawBorder = PropertyHelper::getBoolPropertyValue(this, "Draw border", "Image");
	if (drawBorder != PropertyHelper::getDoubleProperty(this, "Border width", "Image")->getVisible()) {
		PropertyHelper::getDoubleProperty(this, "Border width", "Image")->setVisible(drawBorder);
		PropertyHelper::getPainterProperty(this, "Border color", "Image")->setVisible(drawBorder);
		updateGrid = true;
	}
	
	getProperties().forceResetUpdateForAllProperties();

	createBlockItem();

	return updateGrid;
}

void EntityBlockImage::createProperties() {
	EntityPropertiesBase* prop = EntityPropertiesBoolean::createProperty("Block", "Lock movement", false, "", getProperties());
	prop->setToolTip("If enabled, the block item cannot be moved by mouse in the scene.");

	EntityPropertiesInteger* intProp = EntityPropertiesInteger::createProperty("Block", "Z Value", 0, ot::GraphicsZValues::MinCustomValue, ot::GraphicsZValues::MaxCustomValue, "", getProperties());
	intProp->setAllowCustomValues(false);
	intProp->setToolTip("Z Value for the block item in the graphics scene. Items with higher Z Values are drawn on top of items with lower Z Values.");

	prop = EntityPropertiesBoolean::createProperty("Image", "Maintain aspect ratio", true, "", getProperties());
	prop->setToolTip("If enabled, the aspect ratio of the image is maintained when resizing the block item.");

	intProp = EntityPropertiesInteger::createProperty("Image", "Width", 0, 0, 10000, "", getProperties());
	intProp->setToolTip("Width of the image in pixels. If set to 0, the original image width is used.");
	intProp->setAllowCustomValues(false);
	intProp->setSuffix("px");

	intProp = EntityPropertiesInteger::createProperty("Image", "Height", 0, 0, 10000, "", getProperties());
	intProp->setToolTip("Height of the image in pixels. If set to 0, the original image height is used.");
	intProp->setAllowCustomValues(false);
	intProp->setSuffix("px");

	prop = EntityPropertiesBoolean::createProperty("Image", "Draw border", false, "", getProperties());
	prop->setToolTip("If enabled, a border is drawn around the image.");

	EntityPropertiesDouble* dblProp = EntityPropertiesDouble::createProperty("Image", "Border width", 1., 0.1, 100., "", getProperties());
	dblProp->setToolTip("Thickness of the border drawn around the image.");
	dblProp->setSuffix("px");
	dblProp->setAllowCustomValues(false);
	dblProp->setVisible(false);

	prop = EntityPropertiesGuiPainter::createProperty("Image", "Border color", new ot::StyleRefPainter2D(ot::ColorStyleValueEntry::GraphicsItemBorder), "", getProperties());
	prop->setToolTip("Color of the border drawn around the image.");
	prop->setVisible(false);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Data accessors

void EntityBlockImage::setImageEntity(ot::UID _entityID, ot::UID _entityVersion, ot::ImageFileFormat _format) {
	m_imageUID = _entityID;
	m_imageVersion = _entityVersion;
	m_imageFormat = _format;
	m_image.reset();
	m_image = nullptr;

	setModified();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Property accessors

void EntityBlockImage::addStorageData(bsoncxx::builder::basic::document& _storage) {
	EntityBlock::addStorageData(_storage);

	_storage.append(
		bsoncxx::builder::basic::kvp("ImageDataID", static_cast<int64_t>(m_imageUID)),
		bsoncxx::builder::basic::kvp("ImageDataVersion", static_cast<int64_t>(m_imageVersion)),
		bsoncxx::builder::basic::kvp("ImageFormat", ot::toString(m_imageFormat))
	);
}

void EntityBlockImage::readSpecificDataFromDataBase(const bsoncxx::document::view& _docView, std::map<ot::UID, EntityBase*>& _entityMap) {
	EntityBlock::readSpecificDataFromDataBase(_docView, _entityMap);

	m_imageUID = static_cast<ot::UID>(_docView["ImageDataID"].get_int64());
	m_imageVersion = static_cast<ot::UID>(_docView["ImageDataVersion"].get_int64());
	m_imageFormat = ot::stringToImageFileFormat(_docView["ImageFormat"].get_string().value.data());
}

void EntityBlockImage::ensureFileIsLoaded() {
	if (m_imageUID == ot::invalidUID) {
		OT_LOG_E("No image file has been assigned to this block image entity.");
		return;
	}

	if (m_image != nullptr) {
		return;
	}

	std::map<ot::UID, EntityBase*> entityMap;
	m_image.reset(dynamic_cast<EntityBinaryData*>(readEntityFromEntityIDAndVersion(this, m_imageUID, m_imageVersion, entityMap)));
}
