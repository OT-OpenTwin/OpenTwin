#include "EntityFileText.h"
#include <locale>

#include "OTCommunication/ActionTypes.h"
#include "DataBase.h"
#include "OTCore/String.h"
#include "OTCore/Logger.h"
#include "OTCore/RuntimeTests.h"
#include "OTCore/EncodingGuesser.h"
#include "OTCore/EncodingConverter_UTF16ToUTF8.h"
#include "OTCore/EncodingConverter_ISO88591ToUTF8.h"
#include "OTGui/VisualisationTypes.h"

#include "PropertyHelper.h"

#if OT_TESTING_GLOBAL_AllTestsEnabled==true
#define OT_TESTING_LOCAL_ENTITYFILETEXT_PERFORMANCETEST_ENABLED OT_TESTING_GLOBAL_AllTestsEnabled
#elif OT_TESTING_GLOBAL_RuntimeTestingEnabled==true
#define OT_TESTING_LOCAL_ENTITYFILETEXT_PERFORMANCETEST_ENABLED OT_TESTING_GLOBAL_RuntimeTestingEnabled
#else
#define OT_TESTING_LOCAL_ENTITYFILETEXT_PERFORMANCETEST_ENABLED false
#endif 

#if OT_TESTING_LOCAL_ENTITYFILETEXT_PERFORMANCETEST_ENABLED==true
#define OT_TEST_ENTITYFILETEXT_Interval(___testText) OT_TEST_Interval(ot_intern_entityfiletext_lcl_performancetest, "EntityFileText", ___testText)
#else
#define OT_TEST_ENTITYFILETEXT_Interval(___testText)
#endif

EntityFileText::EntityFileText(ot::UID _ID, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms, ClassFactoryHandler* _factory, const std::string& _owner)
	: EntityFile(_ID,_parent,_obs,_ms,_factory,_owner)
{
}

void EntityFileText::setTextEncoding(ot::TextEncoding::EncodingStandard _encoding)
{
	m_encoding = _encoding;
	EntityPropertiesBase* base=	getProperties().getProperty("Text Encoding");
	if (base != nullptr)
	{
		auto selectionProperty = dynamic_cast<EntityPropertiesSelection*>(base);
		ot::TextEncoding encoding;
		selectionProperty->setValue(encoding.getString(m_encoding));
		selectionProperty->setNeedsUpdate();
	}
	else
	{
		throw std::exception("Property \"Text Encoding\" could not be loaded");
	}
}

ot::TextEncoding::EncodingStandard EntityFileText::getTextEncoding() 
{
	EntityPropertiesBase* base = getProperties().getProperty("Text Encoding");
	if (base != nullptr)
	{
		auto selectionProperty = dynamic_cast<EntityPropertiesSelection*>(base);
		ot::TextEncoding encoding;
		return encoding.getType(selectionProperty->getValue());
	}
	else
	{
		return ot::TextEncoding::UNKNOWN;
	}
}

std::string EntityFileText::getText(void) 
{
	OT_TEST_ENTITYFILETEXT_Interval("Get text");
	const std::vector<char> plainData = getData()->getData();
	std::string textFromBinary(plainData.begin(), plainData.end());

	ot::TextEncoding::EncodingStandard encoding = getTextEncoding();
	if (encoding == ot::TextEncoding::EncodingStandard::UTF8 || encoding == ot::TextEncoding::EncodingStandard::UTF8_BOM)
	{
		return textFromBinary;
	}
	else if (encoding == ot::TextEncoding::EncodingStandard::ANSI)
	{
		ot::EncodingConverter_ISO88591ToUTF8 converter;
		const std::string textAsUTF8 = converter(textFromBinary);
		return textAsUTF8;
	}
	else if (encoding == ot::TextEncoding::EncodingStandard::UTF16_BEBOM || encoding == ot::TextEncoding::EncodingStandard::UTF16_LEBOM)
	{
		ot::EncodingConverter_UTF16ToUTF8 converter;
		const std::string textAsUTF8 = converter(encoding,textFromBinary);
		return textAsUTF8;
	}
	else
	{
		assert(encoding == ot::TextEncoding::EncodingStandard::UNKNOWN);
		OT_LOG_W("Unable to determine the encoding of text file " + getName() + ".");
		return textFromBinary;
	}
}

void EntityFileText::setText(const std::string& _text)
{
	auto dataEntity = getData();
	if (dataEntity != nullptr)
	{
		dataEntity->clearData();
		//Ensure correct encoding at this location!
		dataEntity->setData(_text.data(), _text.size());
		
		if (getModelState() != nullptr)
		{
			dataEntity->StoreToDataBase();
			setData(dataEntity->getEntityID(), dataEntity->getEntityStorageVersion());
			getModelState()->modifyEntityVersion(dataEntity->getEntityID(), dataEntity->getEntityStorageVersion());
			this->StoreToDataBase();
			getModelState()->modifyEntityVersion(this->getEntityID(), this->getEntityStorageVersion());
		}
		else
		{
			setModified();
		}
	}
}

ot::TextEditorCfg EntityFileText::createConfig(bool _includeData) {
	OT_TEST_ENTITYFILETEXT_Interval("Create config");

	ot::TextEditorCfg result;
	result.setEntityName(this->getName());
	result.setTitle(this->getName());
	result.setFileExtensionFilter(this->getFileFilter());

	if (_includeData) {
		result.setPlainText(this->getText());
	}

	const std::string highlight = PropertyHelper::getSelectionPropertyValue(this, "Syntax Highlight", "Text Properties");
	result.setDocumentSyntax(ot::stringToDocumentSyntax(highlight));
	

	return result;
}

ot::ContentChangedHandling EntityFileText::getTextContentChangedHandling()
{
	return m_contentChangedHandlingText;
}

bool EntityFileText::updateFromProperties()
{
	assert(getProperties().anyPropertyNeedsUpdate());
	setModified();

	m_requiresDataUpdate = PropertyHelper::getSelectionProperty(this, "Text Encoding", "Text Properties")->needsUpdate();
	bool needsVisualisationUpdate = PropertyHelper::getSelectionProperty(this, "Syntax Highlight", "Text Properties")->needsUpdate();
	if (m_requiresDataUpdate || needsVisualisationUpdate)
	{
		ot::VisualisationCfg visualisationCfg;
		visualisationCfg.setVisualisationType(OT_ACTION_CMD_UI_TEXTEDITOR_Setup);
		visualisationCfg.setOverrideViewerContent(m_requiresDataUpdate);
		visualisationCfg.setAsActiveView(true);
		getObserver()->requestVisualisation(getEntityID(), visualisationCfg);
	}

	getProperties().forceResetUpdateForAllProperties();

	return false;
}

void EntityFileText::setContentChangedHandling(ot::ContentChangedHandling _contentChangedHandling)
{
	m_contentChangedHandlingText = _contentChangedHandling;
	setModified();
}

void EntityFileText::setSpecializedProperties()
{
	ot::TextEncoding encoding;
	EntityPropertiesSelection::createProperty("Text Properties", "Text Encoding", 
		{ 
			encoding.getString(ot::TextEncoding::ANSI),
			encoding.getString(ot::TextEncoding::UTF8),
			encoding.getString(ot::TextEncoding::UTF8_BOM),
			encoding.getString(ot::TextEncoding::UTF16_BEBOM),
			encoding.getString(ot::TextEncoding::UTF16_LEBOM),
			encoding.getString(ot::TextEncoding::UNKNOWN),
		},
		encoding.getString(m_encoding),
		"default",getProperties());
		
	std::string fileType = getFileType();
	ot::String::toLower(fileType);
	ot::DocumentSyntax defaultSyntaxHighlighting;
	if (fileType == "py")
	{
		defaultSyntaxHighlighting = ot::DocumentSyntax::PythonScript;
	}
	else
	{
		defaultSyntaxHighlighting = ot::DocumentSyntax::PlainText;
	}

	EntityPropertiesSelection::createProperty("Text Properties",
		"Syntax Highlight",
		ot::getSupportedDocumentSyntaxStringList(),
		ot::toString(defaultSyntaxHighlighting),
		"default",
		this->getProperties()
		);
}

bool EntityFileText::requiresDataUpdate()
{
	bool temp = m_requiresDataUpdate;
	m_requiresDataUpdate = false;
	return temp;
}

void EntityFileText::AddStorageData(bsoncxx::builder::basic::document& _storage)
{
	EntityFile::AddStorageData(_storage);
	ot::TextEncoding encoding;
	std::string encodingStr = encoding.getString(m_encoding);
	_storage.append(bsoncxx::builder::basic::kvp("TextEncoding", encodingStr));
	_storage.append(bsoncxx::builder::basic::kvp("ContentChangedHandler", static_cast<int32_t>(m_contentChangedHandlingText)));
}

void EntityFileText::readSpecificDataFromDataBase(bsoncxx::document::view& _doc_view, std::map<ot::UID, EntityBase*>& _entityMap)
{
	EntityFile::readSpecificDataFromDataBase(_doc_view,_entityMap);
	
	const std::string encodingStr(_doc_view["TextEncoding"].get_utf8().value.data());
	int32_t contentChangedHandling = _doc_view["ContentChangedHandler"].get_int32().value;

	if (contentChangedHandling == static_cast<int32_t>(ot::ContentChangedHandling::ModelServiceSavesNotifyOwner))
	{
		m_contentChangedHandlingText = ot::ContentChangedHandling::ModelServiceSavesNotifyOwner;
	}
	else if (contentChangedHandling == static_cast<int32_t>(ot::ContentChangedHandling::OwnerHandles))
	{
		m_contentChangedHandlingText = ot::ContentChangedHandling::OwnerHandles;
	}
	else if (contentChangedHandling == static_cast<int32_t>(ot::ContentChangedHandling::ModelServiceSaves))
	{
		m_contentChangedHandlingText = ot::ContentChangedHandling::ModelServiceSaves;
	}
	else
	{
		assert(false);
	}

	ot::TextEncoding encoding;
	m_encoding = encoding.getType(encodingStr);
}
