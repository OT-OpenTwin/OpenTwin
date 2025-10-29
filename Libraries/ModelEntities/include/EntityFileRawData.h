// @otlicense

#pragma once

// OpenTwin header
#include "EntityFile.h"

class OT_MODELENTITIES_API_EXPORT EntityFileRawData : public EntityFile {
public:
	EntityFileRawData() : EntityFileRawData(0, nullptr, nullptr, nullptr, "") {};
	EntityFileRawData(ot::UID _ID, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms, const std::string& _owner);
	virtual ~EntityFileRawData() = default;

	virtual entityType getEntityType(void) const override { return TOPOLOGY; };
	static std::string className() { return "EntityFileRawData"; };
	virtual std::string getClassName(void) const override { return EntityFileRawData::className(); };

	bool updateFromProperties() override;

protected:
	void setSpecializedProperties() override;

	virtual void addStorageData(bsoncxx::builder::basic::document& _storage) override;
	void readSpecificDataFromDataBase(bsoncxx::document::view& _docView, std::map<ot::UID, EntityBase*>& _entityMap) override;

private:
	ot::ImageFileFormat m_format;

};