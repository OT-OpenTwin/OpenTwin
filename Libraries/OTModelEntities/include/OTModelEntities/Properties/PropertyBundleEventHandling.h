// @otlicense
#pragma once
#include "OTModelEntities/Properties/PropertyBundle.h"
class PropertyBundleEventHandling : public PropertyBundle
{
public:
	void setFolderIDScripts(ot::UID _scriptFolderID) { m_scriptFolderID = _scriptFolderID; }
	void setFolderIDManifests(ot::UID _manifestFolderID) { m_manifestFolderID = _manifestFolderID; }
	void setProperties(EntityBase* _thisObject) override;
	bool updatePropertyVisibility(EntityBase* _thisObject) override;

	std::pair<ot::UID, std::string> getSelectedScript(EntityBase* _thisObject);
	std::pair<ot::UID, std::string> getSelectedManifest(EntityBase* _thisObject);
private:
	const std::string m_group = "Event execution";
	const std::string m_propertyScript = "Script";
	const std::string m_propertyEnvironment = "Environment";
	ot::UID m_scriptFolderID = ot::invalidUID;
	ot::UID m_manifestFolderID = ot::invalidUID;
};
