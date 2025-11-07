// @otlicense
// File: EntityBlockHierarchicalDocumentItem.h
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
#include "IVisualisationText.h"

class EntityBinaryData;

class OT_BLOCKENTITIES_API_EXPORT EntityBlockHierarchicalDocumentItem : public EntityBlock, public IVisualisationText {
public:
	EntityBlockHierarchicalDocumentItem() : EntityBlockHierarchicalDocumentItem(0, nullptr, nullptr, nullptr) {};
	EntityBlockHierarchicalDocumentItem(ot::UID _ID, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms);

	static std::string className() { return "EntityBlockHierarchicalDocumentItem"; }
	virtual std::string getClassName(void) const override { return EntityBlockHierarchicalDocumentItem::className(); };
	virtual entityType getEntityType(void) const override { return TOPOLOGY; }

	virtual std::string getBlockFolderName() const override { return "Documents"; };

	virtual ot::GraphicsItemCfg* createBlockCfg() override;
	virtual bool updateFromProperties() override;

	virtual void createProperties() override;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Data accessors

	void setDocument(const EntityBase& _entity, const std::string& _documentType, const std::string& _documentExtension) { setDocument(_entity.getEntityID(), _entity.getEntityStorageVersion(), _documentType, _documentExtension); };
	void setDocument(ot::UID _entityID, ot::UID _entityVersion, const std::string& _documentType, const std::string& _documentExtension);
	ot::UID getDocumentID() const { return m_documentUID; };
	ot::UID getDocumentVersion() const { return m_documentVersion; };
	std::string getDocumentType() const { return m_documentType; };
	std::string getDocumentExtension() const { return m_documentExtension; };

	// ###########################################################################################################################################################################################################################################################################################################################

	// Text interface

	virtual std::string getText() override;
	virtual void setText(const std::string& _text) override;
	virtual bool visualiseText() override { return false; };
	virtual ot::TextEditorCfg createConfig(bool _includeData) override;
	
	// ###########################################################################################################################################################################################################################################################################################################################

	// Property accessors

protected:
	virtual void addStorageData(bsoncxx::builder::basic::document& _storage) override;
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view& _docView, std::map<ot::UID, EntityBase*>& _entityMap) override;

private:
	void ensureDocumentDataLoaded();

	ot::UID m_documentUID;
	ot::UID m_documentVersion;
	std::string m_documentType;
	std::string m_documentExtension;
	std::shared_ptr<EntityBinaryData> m_documentData;
};
