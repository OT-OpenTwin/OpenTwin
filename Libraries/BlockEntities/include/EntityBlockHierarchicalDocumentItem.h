//! @file EntityBlockHierarchicalDocumentItem.h
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

/*
#include "EntityBlock.h"

class OT_BLOCKENTITIES_API_EXPORT EntityBlockHierarchicalDocumentItem : public EntityBlock {
public:
	EntityBlockHierarchicalDocumentItem() : EntityBlockHierarchicalDocumentItem(0, nullptr, nullptr, nullptr, "") {};
	EntityBlockHierarchicalDocumentItem(ot::UID _ID, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms, const std::string& _owner);

	static std::string className() { return "EntityBlockHierarchicalDocumentItem"; }
	virtual std::string getClassName(void) override { return EntityBlockHierarchicalDocumentItem::className(); };
	virtual entityType getEntityType(void) const override { return TOPOLOGY; }

	virtual ot::GraphicsItemCfg* createBlockCfg() override;
	virtual bool updateFromProperties() override;

	virtual void createProperties();

	// ###########################################################################################################################################################################################################################################################################################################################

	// Data accessors

	void setDocument(ot::UID _entityID, ot::UID _entityVersion);
	ot::UID getDocumentID() const { return m_documentUID; };
	ot::UID getDocumentVersion() const { return m_documentVersion; };
	std::shared_ptr<EntityBase> getPreviewFile();

	// ###########################################################################################################################################################################################################################################################################################################################

	// Property accessors

protected:
	virtual void addStorageData(bsoncxx::builder::basic::document& _storage) override;
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view& _docView, std::map<ot::UID, EntityBase*>& _entityMap) override;

private:
	void ensureDocumentLoaded();

	ot::UID m_documentUID;
	ot::UID m_documentVersion;
	std::shared_ptr<EntityBase> m_documentEntity;
};

*/