#pragma once

#include "OpenTwin/AbstractUIWidgetInterface.h"

#include <akCore/akCore.h>

#include <qstring.h>
#include <qstringlist.h>

#include <list>
#include <map>

#include <Windows.h>

class UiPluginComponent;
class AppBase;

namespace ot { class ServiceBase; }

class ExternalUiPlugin {
public:
	typedef bool(__stdcall *PluginAttachFunctionType)(ot::AbstractUIWidgetInterface * _interface);
	typedef void(__stdcall *PluginDetachFunctionType)(void);
	typedef bool(__stdcall *PluginMessageFunctionType)(const char * _action, const char * _message);

	ExternalUiPlugin(
		const QString&					_name,
		const QString&					_path,
		HINSTANCE						_libraryInstance,
		PluginAttachFunctionType		_attachFunctionRef,
		PluginDetachFunctionType		_detachFunctionRef,
		PluginMessageFunctionType		_messageFunctionRef,
		ot::ServiceBase *				_owner,
		UiPluginComponent *				_component,
		ak::UID							_uid
	);
	ExternalUiPlugin(const ExternalUiPlugin& _other);
	ExternalUiPlugin& operator = (const ExternalUiPlugin& _other);
	virtual ~ExternalUiPlugin();

	const QString& name(void) const { return m_name; }
	const QString& path(void) const { return m_path; }
	HINSTANCE libraryInstance(void) { return m_libraryInstance; }
	PluginAttachFunctionType attachFunction(void) { return m_attachFunctionRef; }
	PluginDetachFunctionType detachFunction(void) { return m_detachFunctionRef; }
	PluginMessageFunctionType messageFunction(void) { return m_messageFunctionRef; }
	ot::ServiceBase * owner(void) { return m_owner; }
	ak::UID uid(void) const { return m_uid; }

	bool forwardMessageToPlugin(const std::string& _action, const std::string& _message);

private:

	QString							m_name;
	QString							m_path;
	HINSTANCE						m_libraryInstance;
	PluginAttachFunctionType		m_attachFunctionRef;
	PluginDetachFunctionType		m_detachFunctionRef;
	PluginMessageFunctionType		m_messageFunctionRef;
	ot::ServiceBase *				m_owner;
	ak::UID							m_uid;
	UiPluginComponent *				m_component;

	ExternalUiPlugin() = delete;
};

class UiPluginManager {
public:
	UiPluginManager(AppBase * _app);
	virtual ~UiPluginManager();

	// ############################################################################################

	// Loader and plugin management

	unsigned long long loadPlugin(const QString& _pluginName, const QString& _filename, ot::ServiceBase * _owner);

	void unloadPlugin(unsigned long long _pluginUID);
	void unloadPlugin(const QString& _pluginName, ot::ServiceBase * _owner);
	void unloadPlugins(ot::ServiceBase * _owner);

	void addPluginSearchPath(const QString& _path);

	bool forwardMessageToPlugin(unsigned long long _pluginUid, const std::string& _action, const std::string& _message);

private:

	void logInfo(const QString& _message);
	void logDebug(const QString& _message);

	//! @brief Will look out for the plugin in the specified search paths
	//! Will return the provided filename if the filename contains the full file path
	QString findPlugin(const QString& _filename);

	AppBase *								m_app;
	QStringList								m_searchPaths;
	ak::UID									m_currentPluginUid;
	std::map<ak::UID, ExternalUiPlugin *>	m_plugins;

	UiPluginManager() = delete;
	UiPluginManager(UiPluginManager&) = delete;
	UiPluginManager& operator = (UiPluginManager&) = delete;
};