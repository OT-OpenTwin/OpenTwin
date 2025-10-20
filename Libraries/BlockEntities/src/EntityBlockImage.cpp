//! @file EntityBlockImage.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "EntityFile.h"
#include "PropertyHelper.h"
#include "EntityFileImage.h"
#include "EntityProperties.h"
#include "EntityBlockImage.h"
#include "OTGui/GraphicsImageItemCfg.h"

static EntityFactoryRegistrar<EntityBlockImage> registrar(EntityBlockImage::className());

EntityBlockImage::EntityBlockImage(ot::UID _ID, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms, const std::string& _owner)
	: EntityBlock(_ID, _parent, _obs, _ms, _owner), m_imageUID(ot::invalidUID), m_imageVersion(ot::invalidUID), m_image(nullptr) {
	OldTreeIcon icon;
	icon.visibleIcon = "Tree/Image";
	icon.hiddenIcon = "Tree/Image";
	setNavigationTreeIcon(icon);

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
	cfg->setImageData(m_image->getImage(), m_image->getImageFormat());
	int width = PropertyHelper::getIntegerPropertyValue(this, "Width", "Image");
	int height = PropertyHelper::getIntegerPropertyValue(this, "Height", "Image");

	if (width > 0 && height > 0) {
		cfg->setFixedSize(width, height);
	}
	cfg->setMaintainAspectRatio(PropertyHelper::getBoolPropertyValue(this, "Maintain aspect ratio", "Image"));
	cfg->setZValue(PropertyHelper::getIntegerPropertyValue(this, "Z Value", "Block"));

	return cfg.release();
}

bool EntityBlockImage::updateFromProperties() {
	getProperties().forceResetUpdateForAllProperties();

	createBlockItem();

	return false;
}

void EntityBlockImage::createProperties() {
	EntityPropertiesBase* prop = EntityPropertiesBoolean::createProperty("Block", "Lock movement", false, "", getProperties());
	prop->setToolTip("If enabled, the block item cannot be moved by mouse in the scene.");

	prop = EntityPropertiesInteger::createProperty("Block", "Z Value", 0, ot::GraphicsZValues::MinCustomValue, ot::GraphicsZValues::MaxCustomValue, "", getProperties());
	prop->setToolTip("Z Value for the block item in the graphics scene. Items with higher Z Values are drawn on top of items with lower Z Values.");

	prop = EntityPropertiesBoolean::createProperty("Image", "Maintain aspect ratio", true, "", getProperties());
	prop->setToolTip("If enabled, the aspect ratio of the image is maintained when resizing the block item.");

	EntityPropertiesInteger* intProp = EntityPropertiesInteger::createProperty("Image", "Width", 0, 0, 10000, "", getProperties());
	intProp->setToolTip("Width of the image in pixels. If set to 0, the original image width is used.");
	intProp->setSuffix("px");

	intProp = EntityPropertiesInteger::createProperty("Image", "Height", 0, 0, 10000, "", getProperties());
	intProp->setToolTip("Height of the image in pixels. If set to 0, the original image height is used.");
	intProp->setSuffix("px");
}

// ###########################################################################################################################################################################################################################################################################################################################

// Data accessors

void EntityBlockImage::setImageEntity(ot::UID _entityID, ot::UID _entityVersion) {
	m_imageUID = _entityID;
	m_imageVersion = _entityVersion;
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
		bsoncxx::builder::basic::kvp("ImageDataVersion", static_cast<int64_t>(m_imageVersion))
	);
}

void EntityBlockImage::readSpecificDataFromDataBase(bsoncxx::document::view& _docView, std::map<ot::UID, EntityBase*>& _entityMap) {
	EntityBlock::readSpecificDataFromDataBase(_docView, _entityMap);

	m_imageUID = static_cast<ot::UID>(_docView["ImageDataID"].get_int64());
	m_imageVersion = static_cast<ot::UID>(_docView["ImageDataVersion"].get_int64());
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
	m_image.reset(dynamic_cast<EntityFileImage*>(readEntityFromEntityIDAndVersion(this, m_imageUID, m_imageVersion, entityMap)));
}
