#pragma once

#include "OpenTwin/AbstractUIWidgetInterface.h"

class ExternalUiPlugin;

class UiPluginComponent : public ot::AbstractUIWidgetInterface {
public:
	UiPluginComponent(ExternalUiPlugin * _plugin);
	virtual ~UiPluginComponent();

	// #########################################################################################

	// Base class functions

	virtual void sendMessageToService(const std::string& _message) override;

	virtual void appenInfoMessage(const QString& _message) override;

	virtual void appenDebugMessage(const QString& _message) override;

	virtual bool addNewTab(const QString& _tabTitle, QWidget * _widget) override;

	// #########################################################################################

	void setPlugin(ExternalUiPlugin * _plugin) { m_plugin = _plugin; }

private:
	ExternalUiPlugin *		m_plugin;

	UiPluginComponent() = delete;
	UiPluginComponent(UiPluginComponent&) = delete;
	UiPluginComponent& operator = (UiPluginComponent&) = delete;
};