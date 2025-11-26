#include "EntityPythonManifest.h"
#include "PropertyHelper.h"
#include "OTCommunication/ActionTypes.h"
#include "OTGui/VisualisationTypes.h"

static EntityFactoryRegistrar<EntityPythonManifest> registrar(EntityPythonManifest::className());

void EntityPythonManifest::addVisualizationNodes()
{
	OldTreeIcon treeIcons;
	treeIcons.size = 32;
	treeIcons.visibleIcon = "TextVisible";
	treeIcons.hiddenIcon = "TextHidden";

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_OBJ_AddSceneNode, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_UI_TREE_Name, ot::JsonString(this->getName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, this->getEntityID(), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_IsEditable, this->getEditable(), doc.GetAllocator());

	ot::VisualisationTypes visTypes;
	visTypes.addTextVisualisation();
	visTypes.addToJsonObject(doc, doc.GetAllocator());

	treeIcons.addToJsonDoc(doc);

	getObserver()->sendMessageToViewer(doc);
}

std::string EntityPythonManifest::getText()
{
	return m_manifestText;
}

void EntityPythonManifest::setText(const std::string& _text)
{
	m_manifestText = _text;
	setModified();
}


ot::TextEditorCfg EntityPythonManifest::createConfig(bool _includeData)
{
	ot::TextEditorCfg result;
	result.setEntityName(this->getName());
	result.setTitle(this->getName());
	
	if (_includeData) {
		result.setPlainText(this->getText());
	}

	return result;
}

void EntityPythonManifest::setManifestID(ot::UID _id)
{
	m_manifestID = _id;
	setModified();
}
void EntityPythonManifest::addStorageData(bsoncxx::builder::basic::document& _storage)
{
	EntityBase::addStorageData(_storage);
	_storage.append(bsoncxx::builder::basic::kvp("ManifestID", static_cast<int64_t>(m_manifestID)));
	_storage.append(bsoncxx::builder::basic::kvp("ManifestText", m_manifestText));
}

void EntityPythonManifest::readSpecificDataFromDataBase(bsoncxx::document::view& _doc_view, std::map<ot::UID, EntityBase*>& _entityMap)
{
	EntityBase::readSpecificDataFromDataBase(_doc_view, _entityMap);
	m_manifestID = static_cast<ot::UID>(_doc_view["ManifestID"].get_int64().value);
	m_manifestText = _doc_view["ManifestText"].get_utf8().value.data();
}



