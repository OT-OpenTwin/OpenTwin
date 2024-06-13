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
#include "OTWidgets/PropertyGrid.h"
#include "OTWidgets/PropertyDialog.h"
#include "OTWidgets/PropertyGridItem.h"
#include "OTWidgets/GlobalColorStyle.h"
#include "OTWidgets/ApplicationPropertiesManager.h"

SettingsManager::SettingsManager(AppBase* _app)
	: m_currentColorStyle(ot::ColorStyleName::BrightStyle), m_app(_app)
{
	this->updateGeneralSettings();

	this->connect(&ot::ApplicationPropertiesManager::instance(), &ot::ApplicationPropertiesManager::propertyChanged, this, &SettingsManager::slotPropertyChanged);
	this->connect(&ot::ApplicationPropertiesManager::instance(), &ot::ApplicationPropertiesManager::propertyDeleteRequested, this, &SettingsManager::slotPropertyDeleteRequested);
}

SettingsManager::~SettingsManager() {

}

bool SettingsManager::showDialog(void) {
	bool ok = ot::ApplicationPropertiesManager::instance().showDialog() == ot::Dialog::Ok;
	this->updateSettings();
	return ok;
}

void SettingsManager::slotPropertyChanged(const std::string& _owner, const ot::Property* const _property) {
	std::string propertyPath = _property->getPropertyPath();

	OT_LOG_I("Property change { \"Owner\": \"" + _owner + "\", \"Property\": \"" + propertyPath + "\" }");

	if (_owner == "General") {
		this->generalSettingsChanged(propertyPath, _property);
	}	
	

}

void SettingsManager::slotPropertyDeleteRequested(const std::string& _owner, const ot::Property* const _property) {

}

void SettingsManager::generalSettingsChanged(const std::string& _propertyPath, const ot::Property* const _property) {
	// Check default application settings
	if (_propertyPath == "Appearance/Color Style") {
		const ot::PropertyStringList* actualProperty = dynamic_cast<const ot::PropertyStringList*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Invalid property { \"Property\": \"" + _propertyPath + "\" }");
			return;
		}

		ot::GlobalColorStyle::instance().setCurrentStyle(actualProperty->getCurrent());
		m_app->createSettingsInstance()->setValue("ColorStyle", QString::fromStdString(actualProperty->getCurrent()));
	}
}

void SettingsManager::updateSettings(void) {
	this->updateGeneralSettings();
}

void SettingsManager::updateGeneralSettings(void) {
	// Create default application settings
	ot::PropertyGridCfg config;
	ot::PropertyGroup* generalGroup = new ot::PropertyGroup("Appearance");

	std::string currentStyle = ot::toString(ot::ColorStyleName::BrightStyle);
	std::list<std::string> options;
	options.push_back(ot::toString(ot::ColorStyleName::BrightStyle));
	options.push_back(ot::toString(ot::ColorStyleName::DarkStyle));
	options.push_back(ot::toString(ot::ColorStyleName::BlueStyle));

	currentStyle = m_app->createSettingsInstance()->value("ColorStyle", QString::fromStdString(currentStyle)).toString().toStdString();
	bool styleSet = false;
	for (const std::string& opt : options) {
		if (opt == currentStyle) {
			if (ot::GlobalColorStyle::instance().hasStyle(currentStyle)) {
				ot::GlobalColorStyle::instance().blockSignals(true);
				ot::GlobalColorStyle::instance().setCurrentStyle(currentStyle);
				ot::GlobalColorStyle::instance().blockSignals(false);
				styleSet = true;
				break;
			}
			else {
				OT_LOG_W("Style does not exist in the GlobalColorStyle: \"" + currentStyle + "\"");
			}
		}
	}

	if (!styleSet) {
		OT_LOG_W("Defaulting style");
		if (!ot::GlobalColorStyle::instance().hasStyle(ot::toString(ot::ColorStyleName::BrightStyle))) {
			OT_LOG_E("Default bright style does not exist");
		}
		else {
			ot::GlobalColorStyle::instance().blockSignals(true);
			currentStyle = ot::toString(ot::ColorStyleName::BrightStyle);
			ot::GlobalColorStyle::instance().setCurrentStyle(currentStyle);
			ot::GlobalColorStyle::instance().blockSignals(false);
		}
	}

	generalGroup->addProperty(new ot::PropertyStringList("Color Style", currentStyle, options));
	config.addRootGroup(generalGroup);

	ot::ApplicationPropertiesManager::instance().add("General", config);
}