//! @file SettingsManager.cpp
//! @author Alexander Kuester (alexk95)
//! @date June 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OToolkit header
#include "AppBase.h"
#include "SettingsManager.h"

// OpenTwin header
#include "OTGui/Property.h"
#include "OTGui/PropertyGroup.h"
#include "OTGui/ColorStyleTypes.h"
#include "OTGui/PropertyStringList.h"
#include "OTWidgets/GlobalColorStyle.h"
#include "OTWidgets/ApplicationPropertiesManager.h"

SettingsManager::SettingsManager(AppBase* _app)
	: m_currentColorStyle(ot::ColorStyleName::BrightStyle), m_app(_app)
{
	// Create default application settings
	ot::PropertyGridCfg config;
	ot::PropertyGroup* generalGroup = new ot::PropertyGroup("Appearance");

	std::string currentStyle = ot::toString(ot::ColorStyleName::BrightStyle);
	std::list<std::string> options;
	options.push_back(ot::toString(ot::ColorStyleName::BrightStyle));
	options.push_back(ot::toString(ot::ColorStyleName::DarkStyle));
	options.push_back(ot::toString(ot::ColorStyleName::BlueStyle));

	std::string lastStyle = m_app->createSettingsInstance()->value("ColorStyle", QString::fromStdString(currentStyle)).toString().toStdString();
	bool styleSet = false;
	for (const std::string& opt : options) {
		if (opt == lastStyle) {
			if (ot::GlobalColorStyle::instance().hasStyle(lastStyle)) {
				ot::GlobalColorStyle::instance().setCurrentStyle(lastStyle);
				styleSet = true;
				break;
			}
			else {
				OT_LOG_W("Style does not exist in the GlobalColorStyle: \"" + lastStyle + "\"");
			}
		}
	}

	if (!styleSet) {
		OT_LOG_W("Defaulting style");
		ot::GlobalColorStyle::instance().setCurrentStyle(ot::toString(ot::ColorStyleName::BrightStyle));
	}

	generalGroup->addProperty(new ot::PropertyStringList("Color Style", currentStyle, options));
	config.addRootGroup(generalGroup);

	ot::ApplicationPropertiesManager::instance().add("General", config);

	this->connect(&ot::ApplicationPropertiesManager::instance(), &ot::ApplicationPropertiesManager::propertyChanged, this, &SettingsManager::slotPropertyChanged);
	this->connect(&ot::ApplicationPropertiesManager::instance(), &ot::ApplicationPropertiesManager::propertyDeleteRequested, this, &SettingsManager::slotPropertyDeleteRequested);
}

SettingsManager::~SettingsManager() {

}

bool SettingsManager::showDialog(void) {
	return ot::ApplicationPropertiesManager::instance().showDialog() == ot::Dialog::Ok;
}

void SettingsManager::slotPropertyChanged(const std::string& _groupPath, const std::string& _propertyName) {
	OT_LOG_I("Property change { \"Group\": \"" + _groupPath + "\", \"Property\": \"" + _propertyName + "\" }");
}

void SettingsManager::slotPropertyDeleteRequested(const std::string& _groupPath, const std::string& _propertyName) {

}