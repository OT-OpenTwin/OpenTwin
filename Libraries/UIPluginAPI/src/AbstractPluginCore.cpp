#include "openTwin/AbstractPluginCore.h"
#include "openTwin/AbstractUIWidgetInterface.h"
#include "OpenTwinCore/ServiceBase.h"

ot::AbstractPluginCore::AbstractPluginCore() 
	: m_uiInterface(nullptr)
{

}

ot::AbstractPluginCore::~AbstractPluginCore() {
}

void ot::AbstractPluginCore::logInfo(const std::string& _message) {
	if (m_uiInterface) m_uiInterface->appenInfoMessage(_message);
}

void ot::AbstractPluginCore::logInfo(const QString& _message) {
	if (m_uiInterface) m_uiInterface->appenInfoMessage(_message);
}

void ot::AbstractPluginCore::logDebug(const std::string& _message) {
	if (m_uiInterface) m_uiInterface->appenDebugMessage(_message);
}

void ot::AbstractPluginCore::logDebug(const QString& _message) {
	if (m_uiInterface) m_uiInterface->appenDebugMessage(_message);
}