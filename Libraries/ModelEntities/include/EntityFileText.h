#pragma once
#include "EntityFile.h"
#include "OTCore/TextEncoding.h"
#include "OTGui/TextEditorCfg.h"
#include "IVisualisationText.h"

class __declspec(dllexport) EntityFileText : public EntityFile, public IVisualisationText
{
public:
	EntityFileText() : EntityFileText(0, nullptr, nullptr, nullptr, "") {};
	EntityFileText(ot::UID _ID, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms, const std::string& _owner);
	virtual ~EntityFileText() = default;

	virtual entityType getEntityType(void) const override { return TOPOLOGY; };
	static std::string className() { return "EntityFileText"; };
	virtual std::string getClassName(void) override { return EntityFileText::className(); };

	void setTextEncoding(ot::TextEncoding::EncodingStandard _encoding);
	ot::TextEncoding::EncodingStandard getTextEncoding();

	//Text visualisation interface
	std::string getText(void) override;
	//! @brief Setting a text will change the underlying data entity. If the modelstate is set, the data entity is directly stored and added to the modelstate.
	void setText(const std::string& _text) override;
	bool visualiseText() override { return true; }
	ot::TextEditorCfg createConfig(bool _includeData) override;
	ot::ContentChangedHandling getTextContentChangedHandling() override;

	bool updateFromProperties() override;

	void setContentChangedHandling(ot::ContentChangedHandling _contentChangedHandling);
protected:
	void setSpecializedProperties() override;
	
	//! @brief Must be called after updateFromProperties
	bool requiresDataUpdate();
	
	virtual void addStorageData(bsoncxx::builder::basic::document& _storage) override;
	void readSpecificDataFromDataBase(bsoncxx::document::view& _doc_view, std::map<ot::UID, EntityBase*>& _entityMap) override;

private:
	bool m_requiresDataUpdate = false;
	ot::ContentChangedHandling m_contentChangedHandlingText = ot::ContentChangedHandling::ModelServiceSaves;
	ot::TextEncoding::EncodingStandard m_encoding = ot::TextEncoding::EncodingStandard::UNKNOWN;
};
