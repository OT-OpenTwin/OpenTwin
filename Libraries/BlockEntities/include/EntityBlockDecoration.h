// @otlicense
// File: EntityBlockDecoration.h
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

#pragma once

#include "EntityBlock.h"

class OT_BLOCKENTITIES_API_EXPORT EntityBlockDecoration : public EntityBlock {
public:
	EntityBlockDecoration() : EntityBlockDecoration(0, nullptr, nullptr, nullptr) {};
	EntityBlockDecoration(ot::UID _ID, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms);

	virtual entityType getEntityType(void) const override { return TOPOLOGY; }

	virtual ot::GraphicsConnectionCfg::ConnectionShape getDefaultConnectionShape() const override { return ot::GraphicsConnectionCfg::ConnectionShape::SmoothLine; };
	virtual bool updateFromProperties() override;

	virtual void createProperties() override;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Property accessors

	void setLockMovement(bool _lock);
	bool getLockMovement() const;

	void setZValue(int _zValue);
	int getZValue() const;

	void setRotation(double _rotation);
	double getRotation() const;

	void setFlipHorizontal(bool _flip);
	bool getFlipHorizontal() const;

	void setFlipVertical(bool _flip);
	bool getFlipVertical() const;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Protected

protected:
	virtual void addStorageData(bsoncxx::builder::basic::document& _storage) override;
	virtual void readSpecificDataFromDataBase(const bsoncxx::document::view& _docView, std::map<ot::UID, EntityBase*>& _entityMap) override;

	void applyDecorationPropertiesToCfg(ot::GraphicsItemCfg* _cfg);
};
