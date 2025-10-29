// @otlicense

#pragma once

// OpenTwin header
#include "EntityFile.h"
#include "IVisualisationImage.h"

class OT_MODELENTITIES_API_EXPORT EntityFileImage : public EntityFile, public IVisualisationImage {
public:
	EntityFileImage() : EntityFileImage(0, nullptr, nullptr, nullptr, "") {};
	EntityFileImage(ot::UID _ID, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms, const std::string& _owner);
	virtual ~EntityFileImage() = default;

	virtual entityType getEntityType(void) const override { return TOPOLOGY; };
	static std::string className() { return "EntityFileImage"; };
	virtual std::string getClassName(void) const override { return EntityFileImage::className(); };

	bool updateFromProperties() override;

	virtual const std::vector<char>& getImage() override;

	void setImageFormat(ot::ImageFileFormat _format) { m_format = _format; setModified(); };
	virtual ot::ImageFileFormat getImageFormat() const override { return m_format; };
	virtual bool visualiseImage() override { return true; };

protected:
	void setSpecializedProperties() override;

	virtual void addStorageData(bsoncxx::builder::basic::document& _storage) override;
	void readSpecificDataFromDataBase(bsoncxx::document::view& _docView, std::map<ot::UID, EntityBase*>& _entityMap) override;	

private:
	ot::ImageFileFormat m_format;

};