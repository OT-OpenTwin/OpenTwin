//! @file EntityBlockHierarchicalProjectItem.h
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

#include "EntityBlock.h"
#include "OTCore/ProjectInformation.h"

class EntityBinaryData;

class OT_BLOCKENTITIES_API_EXPORT EntityBlockHierarchicalProjectItem : public EntityBlock {
public:
	EntityBlockHierarchicalProjectItem() : EntityBlockHierarchicalProjectItem(0, nullptr, nullptr, nullptr, "") {};
	EntityBlockHierarchicalProjectItem(ot::UID _ID, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms, const std::string& _owner);

	static std::string className() { return "EntityBlockHierarchicalProjectItem"; }
	virtual std::string getClassName(void) const override { return EntityBlockHierarchicalProjectItem::className(); };
	virtual entityType getEntityType(void) const override { return TOPOLOGY; }

	virtual ot::GraphicsItemCfg* createBlockCfg() override;
	virtual bool updateFromProperties() override;

	virtual void createProperties();

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

