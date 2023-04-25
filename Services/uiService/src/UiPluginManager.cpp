#include "UiPluginManager.h"
#include "UiPluginComponent.h"
#include "AppBase.h"

#include "OpenTwinCore/otAssert.h"

#include <akAPI/uiAPI.h>
#include <akCore/aAssert.h>
#include <akGui/aColorStyle.h>

#include <qfile.h>

ExternalUiPlugin::ExternalUiPlugin(
	const QString&					_name,
	const QString&					_path,
	HINSTANCE						_libraryInstance,
	PluginAttachFunctionType		_attachFunctionRef,
	PluginDetachFunctionType		_detachFunctionRef,
	PluginMessageFunctionType		_messageFunctionRef,
	PluginStyleFunctionType			_styleFunctionRef,
	ot::ServiceBase *				_owner,
	UiPluginComponent *				_component,
	ak::UID							_uid
)	: m_name(_name), m_path(_path), m_attachFunctionRef(_attachFunctionRef), m_detachFunctionRef(_detachFunctionRef),
	m_messageFunctionRef(_messageFunctionRef), m_styleFunctionRef(_styleFunctionRef),
	m_libraryInstance(_libraryInstance), m_owner(_owner), m_uid(_uid), m_component(_component)
{
	m_component->setPlugin(this);
}

ExternalUiPlugin::ExternalUiPlugin(const ExternalUiPlugin& _other) 
	: m_name(_other.m_name), m_path(_other.m_path), m_attachFunctionRef(_other.m_attachFunctionRef), m_detachFunctionRef(_other.m_detachFunctionRef),
	m_messageFunctionRef(_other.m_messageFunctionRef), m_styleFunctionRef(_other.m_styleFunctionRef),
	m_libraryInstance(_other.m_libraryInstance), m_owner(_other.m_owner), m_uid(_other.m_uid), m_component(_other.m_component)
{
	m_component->setPlugin(this);
}

ExternalUiPlugin& ExternalUiPlugin::operator = (const ExternalUiPlugin& _other) {
	m_name = _other.m_name;
	m_path = _other.m_path;
	m_attachFunctionRef = _other.m_attachFunctionRef;
	m_detachFunctionRef = _other.m_detachFunctionRef;
	m_messageFunctionRef = _other.m_messageFunctionRef;
	m_styleFunctionRef = _other.m_styleFunctionRef;
	m_libraryInstance = _other.m_libraryInstance;
	m_owner = _other.m_owner;
	m_uid = _other.m_uid;
	m_component = _other.m_component;
	m_component->setPlugin(this);
	return *this;
}

ExternalUiPlugin::~ExternalUiPlugin() {}

bool ExternalUiPlugin::forwardMessageToPlugin(const std::string& _action, const std::string& _message) {
	return m_messageFunctionRef(_action.c_str(), _message.c_str());
}

void ExternalUiPlugin::forwardColorStyle(ak::aColorStyle * _colorStyle) {
	m_styleFunctionRef(_colorStyle);
}

// #########################################################################################################################

// #########################################################################################################################

// #########################################################################################################################

UiPluginManager::UiPluginManager(AppBase * _app) : m_app(_app), m_currentPluginUid(0) {}

UiPluginManager::~UiPluginManager() {}

// ############################################################################################

// Loader and plugin management

unsigned long long UiPluginManager::loadPlugin(const QString& _pluginName, const QString& _filename, ot::ServiceBase * _owner) {
	logDebug("Attempring to load UI plugin: " + _pluginName + " \"" + _filename + "\"");

	try {

		// Locate the plugin
		QString filename = findPlugin(_filename);
		if (filename.isEmpty()) {
			logInfo("Failed to load UI plugin");
			aAssert(0, "The specified plugin could not be found");
			return ak::invalidUID;
		}

		// Load the library
		HINSTANCE hGetProcIDDLL = LoadLibrary(filename.toStdWString().c_str());

		if (hGetProcIDDLL) {
			logDebug("Plugin loaded. Resolving functions...");

			// Resolve function address here
			ExternalUiPlugin::PluginAttachFunctionType func_Attach = (ExternalUiPlugin::PluginAttachFunctionType)GetProcAddress(hGetProcIDDLL, "attachToUiService");
			ExternalUiPlugin::PluginDetachFunctionType func_Detach = (ExternalUiPlugin::PluginDetachFunctionType)GetProcAddress(hGetProcIDDLL, "detachFromUiService");
			ExternalUiPlugin::PluginMessageFunctionType func_Message = (ExternalUiPlugin::PluginMessageFunctionType)GetProcAddress(hGetProcIDDLL, "messageReceived");
			ExternalUiPlugin::PluginStyleFunctionType func_Style = (ExternalUiPlugin::PluginStyleFunctionType)GetProcAddress(hGetProcIDDLL, "applyColorStyle");

			if (func_Attach && func_Detach && func_Message && func_Style) {
				logDebug("Attaching UI component to plugin...");
				UiPluginComponent * component = new UiPluginComponent(nullptr);
				ExternalUiPlugin * plugin = new ExternalUiPlugin(_pluginName, filename, hGetProcIDDLL, func_Attach, func_Detach, func_Message, func_Style, _owner, component, ++m_currentPluginUid);

				// Attach the plugin component to the loaded plugin
				if (!func_Attach(component)) {
					logInfo("Plugin: Attach function returned false. Cancel plugin loading.");
					FreeLibrary(hGetProcIDDLL);
					delete plugin;
					--m_currentPluginUid;
					return ak::invalidUID;
				}

				logDebug("Plugin attached");

				ak::aColorStyle * cS = ak::uiAPI::getCurrentColorStyle();
				if (cS) {
					logDebug("Forwarding color style...");
					func_Style(cS);
				}

				// Store information
				m_plugins.insert_or_assign(m_currentPluginUid, plugin);

				logDebug("Plugin ready: " + plugin->name() + " (id = " + QString::number(plugin->uid()) + ")");

				return m_currentPluginUid;
			}
			else {
				logInfo("Failed to load plugin: Functions could not be resolved");
				return ak::invalidUID;
			}
		}
		else {
			logInfo("Failed to load plugin");
			return ak::invalidUID;
		}

	}
	catch (const std::exception& _e) {
		logInfo("[ERROR] [PluginLoader] [Exception]: " + QString(_e.what()));
		return ak::invalidUID;
	}
	catch (...) {
		logInfo("[ERROR] [PluginLoader] Unknown error");
		return ak::invalidUID;
	}
}

void UiPluginManager::unloadPlugin(unsigned long long _pluginUID) {
	auto it = m_plugins.find(_pluginUID);
	if (it == m_plugins.end()) {
		otAssert(0, "Unknown plugin UID");
		return;
	}
	it->second->detachFunction();
	if (!FreeLibrary(it->second->libraryInstance())) {
		otAssert(0, "Failed to unload library");
		logInfo("\n[ERROR] [PluginManager]: Failed to unload plugin \"" + it->second->name() + "\"");
		return;
	}
	logDebug("[PluginManager]: Plugin successfully unloaded: \"" + it->second->name());
	delete it->second;
	m_plugins.erase(_pluginUID);
}

void UiPluginManager::unloadPlugin(const QString& _pluginName, ot::ServiceBase * _owner) {
	bool erased{ true };
	while (erased) {
		erased = false;
		for (auto p : m_plugins) {
			if (p.second->name() == _pluginName && p.second->owner() == _owner) {
				p.second->detachFunction();
				if (!FreeLibrary(p.second->libraryInstance())) {
					otAssert(0, "Failed to unload library");
					logInfo("\n[ERROR] [PluginManager]: Failed to unload plugin \"" + _pluginName + "\"");
					return;
				}
				erased = true;
				logDebug("[PluginManager]: Plugin successfully unloaded: \"" + p.second->name());
				delete p.second;
				m_plugins.erase(p.first);
				break;
			}
		}
	}
}

void UiPluginManager::unloadPlugins(ot::ServiceBase * _owner) {
	bool erased{ true };
	while (erased) {
		erased = false;
		for (auto p : m_plugins) {
			if (p.second->owner() == _owner) {
				p.second->detachFunction();
				if (!FreeLibrary(p.second->libraryInstance())) {
					otAssert(0, "Failed to unload library");
					logInfo("\n[ERROR] [PluginManager]: Failed to unload plugin \"" + p.second->name() + "\"");
					return;
				}
				erased = true;
				logDebug("[PluginManager]: Plugin successfully unloaded: \"" + p.second->name());
				delete p.second;
				m_plugins.erase(p.first);
				break;
			}
		}
	}
}

void UiPluginManager::addPluginSearchPath(const QString& _path) {
	if (_path.isEmpty()) return;
	QString path = _path;
	path = path.replace("/", "\\");
	if (!path.endsWith("\\")) path.append("\\");
	m_searchPaths.push_back(path);
}

bool UiPluginManager::forwardMessageToPlugin(unsigned long long _pluginUid, const std::string& _action, const std::string& _message) {
	auto it = m_plugins.find(_pluginUid);
	if (it == m_plugins.end()) {
		m_app->appendInfoMessage("[ERROR] Failed to forward action (" + QString::fromStdString(_action) + ") to plugin: Unknown plugin UID: " + QString::number(_pluginUid));
		return false;
	}
	return it->second->forwardMessageToPlugin(_action, _message);
}

void UiPluginManager::forwardColorStyle(ak::aColorStyle * _colorStyle) {
	if (_colorStyle) {
		for (auto plugin : m_plugins) {
			plugin.second->forwardColorStyle(_colorStyle);
		}
	}
}

// ############################################################################################

// Private functions

void UiPluginManager::logInfo(const QString& _message) {
	m_app->appendInfoMessage("[PluginManager]: " + _message + "\n");
}

void UiPluginManager::logDebug(const QString& _message) {
	m_app->appendDebugMessage("[PluginManager] [DEBUG]: " + _message + "\n");
}

QString UiPluginManager::findPlugin(const QString& _filename) {
	QString filename;
	for (auto path : m_searchPaths) {
		filename = path + _filename;
		if (QFile::exists(filename)) { return filename; }
	}
	if (QFile::exists(_filename)) return _filename;
	else return QString();
}