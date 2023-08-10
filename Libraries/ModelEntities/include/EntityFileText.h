#pragma once
#include "EntityFile.h"
#include "OpenTwinCore/TextEncoding.h"

class __declspec(dllexport) EntityFileText : public EntityFile
{
public:
	EntityFileText(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactory* factory, const std::string& owner);

	virtual entityType getEntityType(void) override { return TOPOLOGY; };
	virtual std::string getClassName(void) override { return "EntityFileText"; };

	void setTextEncoding(ot::TextEncoding::EncodingStandard encoding);
	ot::TextEncoding::EncodingStandard getTextEncoding();

protected:
	void setSpecializedProperties() override;
	virtual void AddStorageData(bsoncxx::builder::basic::document& storage) override;
	void readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap) override;

private:
	ot::TextEncoding::EncodingStandard _encoding = ot::TextEncoding::EncodingStandard::UNKNOWN;
};
