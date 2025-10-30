// @otlicense

// OToolkit header
#include "AppBase.h"
#include "SettingsManager.h"

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTGui/Property.h"
#include "OTGui/PropertyPath.h"
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
	else if (_propertyPath == "External Tools/Library Path") {
		const ot::PropertyPath* actualProperty = dynamic_cast<const ot::PropertyPath*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Invalid property { \"Property\": \"" + _propertyPath + "\" }");
			return;
		}

		m_externalToolsPath = QString::fromStdString(actualProperty->getPath());
		m_app->createSettingsInstance()->setValue("ExternalToolsPath", m_externalToolsPath);

		OT_LOG_W("Please restart the OToolkit to apply the changes.");
	}
	else if (_propertyPath == "External Tools/Library Debug Path") {
		const ot::PropertyPath* actualProperty = dynamic_cast<const ot::PropertyPath*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Invalid property { \"Property\": \"" + _propertyPath + "\" }");
			return;
		}

		m_externalToolsDebugPath = QString::fromStdString(actualProperty->getPath());
		m_app->createSettingsInstance()->setValue("ExternalToolsDebugPath", m_externalToolsDebugPath);

		OT_LOG_W("Please restart the OToolkit to apply the changes.");
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

	otoolkit::SettingsRef settingsRef = m_app->createSettingsInstance();

	currentStyle = settingsRef->value("ColorStyle", QString::fromStdString(currentStyle)).toString().toStdString();
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

	ot::PropertyGroup* externalToolsGroup = new ot::PropertyGroup("External Tools");

	m_externalToolsPath = settingsRef->value("ExternalToolsPath", QString()).toString();
	m_externalToolsDebugPath = settingsRef->value("ExternalToolsDebugPath", QString()).toString();

	ot::PropertyPath* libPathProp = new ot::PropertyPath("Library Path", m_externalToolsPath.toStdString(), ot::PathBrowseMode::Directory);
	libPathProp->setPropertyTip("Library path for external tools that will be loaded in the Release mode of the OToolkit.\nTools need to have the *.ottool extension.");
	externalToolsGroup->addProperty(libPathProp);

	ot::PropertyPath* libPathDebugProp = new ot::PropertyPath("Library Debug Path", m_externalToolsDebugPath.toStdString(), ot::PathBrowseMode::Directory);
	libPathDebugProp->setPropertyTip("Library path for external tools that will be loaded in the Debug mode of the OToolkit.\nTools need to have the *.ottoold extension.");
	externalToolsGroup->addProperty(libPathDebugProp);

	config.addRootGroup(externalToolsGroup);

	ot::ApplicationPropertiesManager::instance().add("General", config);
}