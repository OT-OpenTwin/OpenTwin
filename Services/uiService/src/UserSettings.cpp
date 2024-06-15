#include "UserSettings.h"
#include "AppBase.h"

#include "OTCore/OTAssert.h"
#include "OTGui/ColorStyleTypes.h"
#include "OTWidgets/GlobalColorStyle.h"
#include "OTWidgets/ApplicationPropertiesManager.h"

UserSettings& UserSettings::instance(void) {
	static UserSettings g_instance;
	return g_instance;
}

void UserSettings::showDialog(void) {
	ot::ApplicationPropertiesManager::instance().showDialog();
}

void UserSettings::showDialog(const QString& _group) {
	ot::ApplicationPropertiesManager::instance().showDialog();
}

void UserSettings::clear(void) {
	ot::ApplicationPropertiesManager::instance().clear();
}

void UserSettings::addSettings(const std::string& _serviceName, const ot::PropertyGridCfg& _config) {
	ot::ApplicationPropertiesManager::instance().add(_serviceName, _config);
}

// #######################################################################################

// Slots

void UserSettings::slotItemChanged(const std::string& _owner, const ot::Property* _property) {
	AppBase::instance()->settingsChanged(_owner, _property);
}

// #######################################################################################

// Private member

UserSettings::UserSettings() {
	this->connect(&ot::ApplicationPropertiesManager::instance(), &ot::ApplicationPropertiesManager::propertyChanged, this, &UserSettings::slotItemChanged);
}
