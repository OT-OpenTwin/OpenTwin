#pragma once
#include "EntityFile.h"
#include "OTCore/TextEncoding.h"
#include "OTGui/TextEditorCfg.h"
#include "VisualisationInterfaceText.h"

class __declspec(dllexport) EntityFileText : public EntityFile, public VisualisationInterfaceText
{
public:
	EntityFileText(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner);

	virtual entityType getEntityType(void) override { return TOPOLOGY; };
	virtual std::string getClassName(void) override { return "EntityFileText"; };

	void setTextEncoding(ot::TextEncoding::EncodingStandard encoding);
	ot::TextEncoding::EncodingStandard getTextEncoding();

	//Text visualisation interface
	std::string getText(void) override;
	//! @brief Setting a text will change the underlying data entity. If the modelstate is set, the data entity is directly stored and added to the modelstate.
	void setText(const std::string& _text) override;
	bool visualiseText() override { return true; }
	ot::TextEditorCfg createConfig() override;

protected:
	void setSpecializedProperties() override;
	virtual void AddStorageData(bsoncxx::builder::basic::document& storage) override;
	void readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap) override;

private:
	ot::TextEncoding::EncodingStandard _encoding = ot::TextEncoding::EncodingStandard::UNKNOWN;
};
