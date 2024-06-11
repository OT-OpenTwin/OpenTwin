//! @file SettingsManager.h
//! @author Alexander Kuester (alexk95)
//! @date June 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTGui/ColorStyleTypes.h"

// Qt header
#include <QtCore/qobject.h>

// std header
#include <string>

class AppBase;

class SettingsManager : public QObject {
	Q_OBJECT
	OT_DECL_NOCOPY(SettingsManager)
	OT_DECL_NODEFAULT(SettingsManager)
public:
	SettingsManager(AppBase* _app);
	virtual ~SettingsManager();

	bool showDialog(void);

private Q_SLOTS:
	void slotPropertyChanged(const std::string& _groupPath, const std::string& _propertyName);
	void slotPropertyDeleteRequested(const std::string& _groupPath, const std::string& _propertyName);

private:
	void updateSettings(void);
	void updateGeneralSettings(void);

	AppBase* m_app;
	ot::ColorStyleName m_currentColorStyle;
};