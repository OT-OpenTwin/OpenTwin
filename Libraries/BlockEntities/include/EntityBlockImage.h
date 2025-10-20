//! @file EntityBlockImage.h
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

#include "EntityBlock.h"
#include "OTCore/ProjectInformation.h"

class EntityFileImage;

class OT_BLOCKENTITIES_API_EXPORT EntityBlockImage : public EntityBlock {
public:
	EntityBlockImage() : EntityBlockImage(0, nullptr, nullptr, nullptr, "") {};
	EntityBlockImage(ot::UID _ID, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms, const std::string& _owner);

	static std::string className() { return "EntityBlockImage"; }
	virtual std::string getClassName(void) const override { return EntityBlockImage::className(); };
	virtual entityType getEntityType(void) const override { return TOPOLOGY; }

	virtual ot::GraphicsItemCfg* createBlockCfg() override;
	virtual bool updateFromProperties() override;

	virtual void createProperties();

	// ###########################################################################################################################################################################################################################################################################################################################

	// Data accessors

	void setImageEntity(const EntityBase& _entity) { setImageEntity(_entity.getEntityID(), _entity.getEntityStorageVersion()); };
	void setImageEntity(ot::UID _entityID, ot::UID _entityVersion);
	ot::UID getImageEntityID() const { return m_imageUID; };
	ot::UID getImageEntityVersion() const { return m_imageVersion; };

	// ###########################################################################################################################################################################################################################################################################################################################

	// Property accessors

protected:
	virtual void addStorageData(bsoncxx::builder::basic::document& _storage) override;
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view& _docView, std::map<ot::UID, EntityBase*>& _entityMap) override;

private:
	void ensureFileIsLoaded();

	ot::UID m_imageUID;
	ot::UID m_imageVersion;
	std::shared_ptr<EntityFileImage> m_image;
};