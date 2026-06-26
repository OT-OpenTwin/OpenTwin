#include "OTModelEntities/Properties/Bundle/PropertyBundleEventHandling.h"
#include "OTModelEntities/Properties/EntityProperties.h"
#include "OTCore/FolderNames.h"
#include "OTModelEntities/Properties/PropertyHelper.h"

void PropertyBundleEventHandling::setProperties(EntityBase* _thisObject)
{
	assert(m_manifestFolderID != ot::invalidUID && m_scriptFolderID != ot::invalidUID);

	EntityPropertiesExtendedEntityList::createProperty(m_group, m_propertyEnvironment, ot::FolderNames::PythonManifestFolder, m_manifestFolderID,"",-1, { "< Load from Library >" }, { "" }, "default", _thisObject->getProperties());
	EntityPropertiesExtendedEntityList::createProperty(m_group, m_propertyScript, ot::FolderNames::PythonScriptFolder, m_scriptFolderID, "", -1, { "< Load from Library >" }, { "" }, "default", _thisObject->getProperties());
}

bool PropertyBundleEventHandling::updatePropertyVisibility(EntityBase* _thisObject)
{
	return false;
}

std::pair<ot::UID, std::string> PropertyBundleEventHandling::getSelectedScript(EntityBase* _thisObject)
{
	auto propBase = _thisObject->getProperties().getProperty(m_propertyScript);
	EntityPropertiesExtendedEntityList* scriptSelection = dynamic_cast<EntityPropertiesExtendedEntityList*>(propBase);
	assert(scriptSelection != nullptr);
	

	ot::UID selectedID = scriptSelection->getValueID();
	std::string selectedName = scriptSelection->getValueName();
	return { selectedID,selectedName };
}

std::pair<ot::UID, std::string> PropertyBundleEventHandling::getSelectedManifest(EntityBase* _thisObject)
{
	auto propBase = _thisObject->getProperties().getProperty(m_propertyEnvironment);
	EntityPropertiesExtendedEntityList* selectedManifest = dynamic_cast<EntityPropertiesExtendedEntityList*>(propBase);
	assert(selectedManifest != nullptr);

	ot::UID selectedManifestID = selectedManifest->getValueID();
	std::string selectedManifestName = selectedManifest->getValueName();
	return { selectedManifestID,selectedManifestName };
}
