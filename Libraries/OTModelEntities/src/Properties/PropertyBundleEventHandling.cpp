#include "OTModelEntities/Properties/PropertyBundleEventHandling.h"
#include "OTModelEntities/Properties/EntityProperties.h"
#include "OTCore/FolderNames.h"
#include "OTModelEntities/Properties/PropertyHelper.h"
void PropertyBundleEventHandling::setProperties(EntityBase* _thisObject)
{
	assert(m_manifestFolderID != ot::invalidUID && m_scriptFolderID != ot::invalidUID);

	EntityPropertiesEntityList::createProperty(m_group, m_propertyEnvironment, ot::FolderNames::PythonManifestFolder, m_manifestFolderID, "", ot::invalidUID, "default", _thisObject->getProperties());
	EntityPropertiesEntityList::createProperty(m_group, m_propertyScript, ot::FolderNames::PythonScriptFolder, m_scriptFolderID, "", ot::invalidUID, "default", _thisObject->getProperties());
}

bool PropertyBundleEventHandling::updatePropertyVisibility(EntityBase* _thisObject)
{
	return false;
}

std::pair<ot::UID, std::string> PropertyBundleEventHandling::getSelectedScript(EntityBase* _thisObject)
{
	ot::UID selectedID = PropertyHelper::getEntityListPropertyValueID(_thisObject, m_propertyScript, m_group);
	std::string selectedName = PropertyHelper::getEntityListPropertyValueName(_thisObject, m_propertyScript, m_group);
	return { selectedID,selectedName };
}

std::pair<ot::UID, std::string> PropertyBundleEventHandling::getSelectedManifest(EntityBase* _thisObject)
{
	ot::UID selectedManifestID = PropertyHelper::getEntityListPropertyValueID(_thisObject, m_propertyEnvironment, m_group);
	std::string selectedManifestName = PropertyHelper::getEntityListPropertyValueName(_thisObject, m_propertyEnvironment, m_group);
	return { selectedManifestID,selectedManifestName };
}
