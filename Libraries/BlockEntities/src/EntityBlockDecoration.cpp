// @otlicense
// File: EntityBlockDecoration.cpp
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
#include "EntityBlockDecoration.h"

EntityBlockDecoration::EntityBlockDecoration(ot::UID _ID, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms)
	: EntityBlock(_ID, _parent, _obs, _ms)
{

}

bool EntityBlockDecoration::updateFromProperties() {
	bool updateGrid = false;

	getProperties().forceResetUpdateForAllProperties();

	createBlockItem();

	return updateGrid;
}

void EntityBlockDecoration::createProperties() {
	EntityBlock::createProperties();

	EntityPropertiesBase* prop = EntityPropertiesBoolean::createProperty("Block", "Lock Movement", false, "", getProperties());
	prop->setToolTip("If enabled, the block item cannot be moved by mouse in the scene.");

	EntityPropertiesInteger* intProp = EntityPropertiesInteger::createProperty("Block", "Z Value", 0, ot::GraphicsZValues::MinCustomValue, ot::GraphicsZValues::MaxCustomValue, "", getProperties());
	intProp->setAllowCustomValues(false);
	intProp->setToolTip("Z Value for the block item in the graphics scene. Items with higher Z Values are drawn on top of items with lower Z Values.");

	EntityPropertiesDouble* rotationProp = EntityPropertiesDouble::createProperty("Block", "Rotation", 0., 0., 359.99, "", getProperties());
	rotationProp->setToolTip("Rotation of the block decoration in degrees.");

	EntityPropertiesBoolean* flipHProp = EntityPropertiesBoolean::createProperty("Block", "Flip Horizontal", false, "", getProperties());
	EntityPropertiesBoolean* flipVProp = EntityPropertiesBoolean::createProperty("Block", "Flip Vertical", false, "", getProperties());
}

// ###########################################################################################################################################################################################################################################################################################################################

// Property accessors

void EntityBlockDecoration::setLockMovement(bool _lock) {
	PropertyHelper::setBoolPropertyValue(_lock, this, "Lock Movement", "Block");
}

bool EntityBlockDecoration::getLockMovement() const {
	return PropertyHelper::getBoolPropertyValue(this, "Lock Movement", "Block");
}

void EntityBlockDecoration::setZValue(int _zValue) {
	PropertyHelper::setIntegerPropertyValue(_zValue, this, "Z Value", "Block");
}

int EntityBlockDecoration::getZValue() const {
	return PropertyHelper::getIntegerPropertyValue(this, "Z Value", "Block");
}

void EntityBlockDecoration::setRotation(double _rotation) {
	PropertyHelper::setDoublePropertyValue(_rotation, this, "Rotation", "Block");
}

double EntityBlockDecoration::getRotation() const {
	return PropertyHelper::getDoublePropertyValue(this, "Rotation", "Block");
}

void EntityBlockDecoration::setFlipHorizontal(bool _flip) {
	PropertyHelper::setBoolPropertyValue(_flip, this, "Flip Horizontal", "Block");
}

bool EntityBlockDecoration::getFlipHorizontal() const {
	return PropertyHelper::getBoolPropertyValue(this, "Flip Horizontal", "Block");
}

void EntityBlockDecoration::setFlipVertical(bool _flip) {
	PropertyHelper::setBoolPropertyValue(_flip, this, "Flip Vertical", "Block");
}

bool EntityBlockDecoration::getFlipVertical() const {
	return PropertyHelper::getBoolPropertyValue(this, "Flip Vertical", "Block");
}

// ###########################################################################################################################################################################################################################################################################################################################

// Protected

void EntityBlockDecoration::addStorageData(bsoncxx::builder::basic::document& _storage) {
	EntityBlock::addStorageData(_storage);
}

void EntityBlockDecoration::readSpecificDataFromDataBase(const bsoncxx::document::view& _docView, std::map<ot::UID, EntityBase*>& _entityMap) {
	EntityBlock::readSpecificDataFromDataBase(_docView, _entityMap);
}

void EntityBlockDecoration::applyDecorationPropertiesToCfg(ot::GraphicsItemCfg* _cfg) {
	_cfg->setGraphicsItemFlag(ot::GraphicsItemCfg::ItemIsMoveable, !getLockMovement());
	_cfg->setZValue(getZValue());

	ot::Transform transform;
	transform.setRotation(getRotation());
	transform.setFlipState(ot::Transform::FlipState::FlipHorizontally, getFlipHorizontal());
	transform.setFlipState(ot::Transform::FlipState::FlipVertically, getFlipVertical());
	_cfg->setTransform(transform);
}
