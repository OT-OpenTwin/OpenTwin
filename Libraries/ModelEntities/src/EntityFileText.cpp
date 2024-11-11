#include "EntityFileText.h"
#include <locale>
#include "OTCommunication/ActionTypes.h"
#include "DataBase.h"
#include "OTCore/EncodingGuesser.h"
#include "OTCore/EncodingConverter_ISO88591ToUTF8.h"
#include "OTCore/EncodingConverter_UTF16ToUTF8.h"
#include "OTCore/Logger.h"

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

void EntityFileText::addVisualizationNodes()
{
	TreeIcon treeIcons;
	treeIcons.size = 32;
	treeIcons.visibleIcon = "TextVisible";
	treeIcons.hiddenIcon = "TextHidden";

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_OBJ_AddText, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_UI_TREE_Name, ot::JsonString(this->getName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, this->getEntityID(), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_IsEditable, this->getEditable(), doc.GetAllocator());
	
	treeIcons.addToJsonDoc(doc);

	getObserver()->sendMessageToViewer(doc);
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
			getModelState()->addNewEntity(dataEntity->getEntityID(), this->getEntityID(), dataEntity->getEntityStorageVersion(), ModelStateEntity::tEntityType::DATA);
			setData(dataEntity->getEntityID(), dataEntity->getEntityStorageVersion());
		}

	}
}

ot::TextEditorCfg EntityFileText::createConfig(void) {
	ot::TextEditorCfg result;
	result.setName(this->getName());
	result.setTitle(this->getName());
	result.setPlainText(this->getText());

	const std::string fileType = getFileType();
	if (fileType == "py")
	{
		result.setDocumentSyntax(ot::DocumentSyntax::PythonScript);
	}
	else
	{
		result.setDocumentSyntax(ot::DocumentSyntax::PlainText);
	}

	return result;
}

ot::ContentChangedHandling EntityFileText::getContentChangedHandling()
{
	return m_contentChangedHandling;
}

void EntityFileText::setContentChangedHandling(ot::ContentChangedHandling _contentChangedHandling)
{
	m_contentChangedHandling = _contentChangedHandling; 
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

	EntityPropertiesSelection::createProperty("Text Properties",
		"Syntax Highlight",
		ot::getAllSupportedDocumentSyntax(),
		ot::toString(ot::DocumentSyntax::PlainText),
		"default",
		this->getProperties()
		);
}

void EntityFileText::AddStorageData(bsoncxx::builder::basic::document& _storage)
{
	EntityFile::AddStorageData(_storage);
	ot::TextEncoding encoding;
	std::string encodingStr = encoding.getString(m_encoding);
	_storage.append(bsoncxx::builder::basic::kvp("TextEncoding", encodingStr));
	_storage.append(bsoncxx::builder::basic::kvp("ContentChangedHandler", static_cast<int32_t>(m_contentChangedHandling)));
}

void EntityFileText::readSpecificDataFromDataBase(bsoncxx::document::view& _doc_view, std::map<ot::UID, EntityBase*>& _entityMap)
{
	EntityFile::readSpecificDataFromDataBase(_doc_view,_entityMap);
	
	const std::string encodingStr(_doc_view["TextEncoding"].get_utf8().value.data());
	int32_t contentChangedHandling = _doc_view["ContentChangedHandler"].get_int32().value;

	if (contentChangedHandling == static_cast<int32_t>(ot::ContentChangedHandling::NotifyOwner))
	{
		m_contentChangedHandling = ot::ContentChangedHandling::NotifyOwner;
	}
	else if (contentChangedHandling == static_cast<int32_t>(ot::ContentChangedHandling::OwnerHandles))
	{
		m_contentChangedHandling = ot::ContentChangedHandling::OwnerHandles;
	}
	else if (contentChangedHandling == static_cast<int32_t>(ot::ContentChangedHandling::SimpleSave))
	{
		m_contentChangedHandling = ot::ContentChangedHandling::SimpleSave;
	}
	else
	{
		assert(false);
	}

	ot::TextEncoding encoding;
	m_encoding = encoding.getType(encodingStr);
}
