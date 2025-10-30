// @otlicense

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

	virtual void createProperties();

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