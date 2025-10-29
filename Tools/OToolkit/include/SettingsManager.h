// @otlicense

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

namespace ot { class Property; }

class SettingsManager : public QObject {
	Q_OBJECT
	OT_DECL_NOCOPY(SettingsManager)
	OT_DECL_NODEFAULT(SettingsManager)
public:
	SettingsManager(AppBase* _app);
	virtual ~SettingsManager();

	bool showDialog(void);

	const QString& getExternalToolsPath(void) const { return m_externalToolsPath; };
	const QString& getExternalToolsDebugPath(void) const { return m_externalToolsDebugPath; };

private Q_SLOTS:
	void slotPropertyChanged(const std::string& _owner, const ot::Property* const _property);
	void slotPropertyDeleteRequested(const std::string& _owner, const ot::Property* const _property);

private:
	void generalSettingsChanged(const std::string& _propertyPath, const ot::Property* const _property);
	void updateSettings(void);
	void updateGeneralSettings(void);

	AppBase* m_app;
	ot::ColorStyleName m_currentColorStyle;
	QString m_externalToolsPath;
	QString m_externalToolsDebugPath;
};