// @otlicense
// File: EntityBlockImage.h
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
#include "OTCore/ProjectInformation.h"

class EntityBinaryData;

class OT_BLOCKENTITIES_API_EXPORT EntityBlockImage : public EntityBlock {
public:
	EntityBlockImage() : EntityBlockImage(0, nullptr, nullptr, nullptr, "") {};
	EntityBlockImage(ot::UID _ID, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms, const std::string& _owner);

	static std::string className() { return "EntityBlockImage"; }
	virtual std::string getClassName(void) const override { return EntityBlockImage::className(); };
	virtual entityType getEntityType(void) const override { return TOPOLOGY; }

	virtual ot::GraphicsItemCfg* createBlockCfg() override;
	virtual bool updateFromProperties() override;

	virtual void createProperties() override;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Data accessors

	void setImageEntity(const EntityBase& _entity, ot::ImageFileFormat _format) { setImageEntity(_entity.getEntityID(), _entity.getEntityStorageVersion(), _format); };
	void setImageEntity(ot::UID _entityID, ot::UID _entityVersion, ot::ImageFileFormat _format);
	ot::UID getImageEntityID() const { return m_imageUID; };
	ot::UID getImageEntityVersion() const { return m_imageVersion; };
	ot::ImageFileFormat getImageFormat() const { return m_imageFormat; };

	// ###########################################################################################################################################################################################################################################################################################################################

	// Property accessors

protected:
	virtual void addStorageData(bsoncxx::builder::basic::document& _storage) override;
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view& _docView, std::map<ot::UID, EntityBase*>& _entityMap) override;

private:
	void ensureFileIsLoaded();

	ot::UID m_imageUID;
	ot::UID m_imageVersion;
	ot::ImageFileFormat m_imageFormat;
	std::shared_ptr<EntityBinaryData> m_image;
};