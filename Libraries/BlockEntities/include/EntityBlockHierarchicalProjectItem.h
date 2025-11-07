// @otlicense
// File: EntityBlockHierarchicalProjectItem.h
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

class OT_BLOCKENTITIES_API_EXPORT EntityBlockHierarchicalProjectItem : public EntityBlock {
public:
	EntityBlockHierarchicalProjectItem() : EntityBlockHierarchicalProjectItem(0, nullptr, nullptr, nullptr) {};
	EntityBlockHierarchicalProjectItem(ot::UID _ID, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms);

	static std::string className() { return "EntityBlockHierarchicalProjectItem"; }
	virtual std::string getClassName(void) const override { return EntityBlockHierarchicalProjectItem::className(); };
	virtual entityType getEntityType(void) const override { return TOPOLOGY; }

	virtual ot::GraphicsItemCfg* createBlockCfg() override;
	virtual bool updateFromProperties() override;

	virtual void createProperties() override;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Data accessors

	void setPreviewFile(const EntityBase& _entity, ot::ImageFileFormat _format) { setPreviewFile(_entity.getEntityID(), _entity.getEntityStorageVersion(), _format); };
	void setPreviewFile(ot::UID _entityID, ot::UID _entityVersion, ot::ImageFileFormat _format);
	void removePreviewFile();
	bool hasPreviewFile() const { return m_previewUID != ot::invalidUID; };
	ot::UID getPreviewFileID() const { return m_previewUID; };
	ot::UID getPreviewFileVersion() const { return m_previewVersion; };
	ot::ImageFileFormat getPreviewFileFormat() const { return m_previewFormat; };
	std::shared_ptr<EntityBinaryData> getPreviewFileData();

	// ###########################################################################################################################################################################################################################################################################################################################

	// Property accessors

	void setProjectInformation(const ot::ProjectInformation& _info);
	ot::ProjectInformation getProjectInformation() const;

	void setUseLatestVersion(bool _flag);
	bool getUseLatestVersion() const;

	void setCustomVersion(const std::string& _version);
	std::string getCustomVersion() const;

protected:
	virtual void addStorageData(bsoncxx::builder::basic::document& _storage) override;
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view& _docView, std::map<ot::UID, EntityBase*>& _entityMap) override;

private:
	void ensurePreviewLoaded();

	std::string m_projectName;
	std::string m_projectType;
	std::string m_collectionName;

	ot::UID m_previewUID;
	ot::UID m_previewVersion;
	ot::ImageFileFormat m_previewFormat;
	std::shared_ptr<EntityBinaryData> m_previewData;
};

