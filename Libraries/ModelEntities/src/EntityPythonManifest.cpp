#include "EntityPythonManifest.h"
#include "PropertyHelper.h"
#include "OTCommunication/ActionTypes.h"
#include "OTGui/VisualisationTypes.h"
#include "OTCore/String.h"

static EntityFactoryRegistrar<EntityPythonManifest> registrar(EntityPythonManifest::className());

EntityPythonManifest::EntityPythonManifest(ot::UID _ID, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms)
	: EntityBase(_ID, _parent, _obs, _ms) 
{
	ot::EntityTreeItem treeItem = getTreeItem();
	treeItem.setVisibleIcon("Default/TextVisible");
	treeItem.setHiddenIcon("Default/TextHidden");
	setDefaultTreeItem(treeItem);
	
	ot::VisualisationTypes visTypes = getVisualizationTypes();
	visTypes.addTextVisualisation();
	this->setDefaultVisualizationTypes(visTypes);
}

void EntityPythonManifest::addVisualizationNodes()
{
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_OBJ_AddSceneNode, doc.GetAllocator()), doc.GetAllocator());

	doc.AddMember(OT_ACTION_PARAM_TreeItem, ot::JsonObject(getTreeItem(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_VisualizationTypes, ot::JsonObject(getVisualizationTypes(), doc.GetAllocator()), doc.GetAllocator());

	getObserver()->sendMessageToViewer(doc);
}

void EntityPythonManifest::setEntityID(ot::UID id)
{
	EntityBase::setEntityID(id);
	if (m_manifestID == ot::invalidUID)
	{
		m_manifestID = id;
	}
}

std::string EntityPythonManifest::getText()
{
	return m_manifestText;
}

void EntityPythonManifest::setText(const std::string& _text)
{
	bool changedEnvironment = environmentHasChanged(_text);
	if (changedEnvironment)
	{
		m_manifestID = createEntityUID();
	}
	m_manifestText = _text;
	setModified();
}


ot::TextEditorCfg EntityPythonManifest::createConfig(const ot::VisualisationCfg& _visualizationConfig)
{
	ot::TextEditorCfg result;
	result.setEntityName(this->getName());
	result.setTitle(this->getName());
	
	if (_visualizationConfig.getOverrideViewerContent()) {
		result.setPlainText(this->getText());
	}

	return result;
}

void EntityPythonManifest::setManifestID(ot::UID _id)
{
	m_manifestID = _id;
	setModified();
}
ot::UID EntityPythonManifest::generateNewManifestID()
{
	m_manifestID = createEntityUID(); 
	setModified();
	return m_manifestID;
}
void EntityPythonManifest::replaceManifest(const std::string& _newManifestText)
{
	m_manifestText = _newManifestText;
	setModified();
}
bool EntityPythonManifest::environmentHasChanged(const std::string& _newContent)
{
	auto currentPackageList = getPackageList(m_manifestText);
	auto newPackageList = getPackageList(_newContent);
	
	return (currentPackageList.has_value() != newPackageList.has_value()) || (currentPackageList.value() != newPackageList.value());
}

std::string EntityPythonManifest::trim(const std::string& _line) {
	size_t start = _line.find_first_not_of(" \t");
	if (start == std::string::npos)
	{
		return "";
	}
	size_t end = _line.find_last_not_of(" \t");
	return _line.substr(start, end - start + 1);
}

std::optional<std::list<std::string>> EntityPythonManifest::getPackageList(const std::string _text)
{
	std::optional<std::list<std::string>> packageList = std::list<std::string>();
	std::stringstream scriptStream(_text);
	std::string line;
	int32_t lineNumber = 0;
	while (std::getline(scriptStream, line))
	{
		lineNumber++;
		line = trim(line);
		ot::String::removeControlCharacters(line);
		ot::String::removeWhitespaces(line);
		// Skip empty lines and comments
		if (line.empty() || line[0] == '#')
		{
			continue;
		}
		// Is porbably a valid line
		auto posEq = line.find("==");
		auto posAt = line.find("@");
		std::string moduleName;
		std::string version;
		if ( posEq!= std::string::npos) 
		{
			moduleName = line.substr(0,posEq);
			version = line.substr(posEq);
		}
		else if(posAt != std::string::npos)
		{
			moduleName = line.substr(0, posAt);
			version = line.substr(posAt);
		}
		if(!moduleName.empty() && !version.empty())
		{
			if (packageList.has_value())
			{
				packageList.value().push_back(line);
			}
		}
		else
		{
			packageList.reset();
			OT_LOG_E("Invalid line in manifest: line" + std::to_string(lineNumber));
		}
	}

	return packageList;
}

void EntityPythonManifest::addStorageData(bsoncxx::builder::basic::document& _storage)
{
	EntityBase::addStorageData(_storage);
	_storage.append(bsoncxx::builder::basic::kvp("ManifestID", static_cast<int64_t>(m_manifestID)));
	_storage.append(bsoncxx::builder::basic::kvp("ManifestText", m_manifestText));
}

void EntityPythonManifest::readSpecificDataFromDataBase(const bsoncxx::document::view& _doc_view, std::map<ot::UID, EntityBase*>& _entityMap)
{
	EntityBase::readSpecificDataFromDataBase(_doc_view, _entityMap);
	m_manifestID = static_cast<ot::UID>(_doc_view["ManifestID"].get_int64().value);
	m_manifestText = _doc_view["ManifestText"].get_utf8().value.data();
}



