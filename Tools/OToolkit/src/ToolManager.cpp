// Toolkit API header
#include "ToolManager.h"
#include "AbstractTool.h"

OToolkitAPI::ToolManager& OToolkitAPI::ToolManager::instance(void) {
	static ToolManager g_instance;
	return g_instance;
}

OToolkitAPI::ToolID OToolkitAPI::ToolManager::add(AbstractTool * _tool) {
	if (_tool == nullptr) return 0;

	_tool->setToolId(++m_currentId);
	m_tools.insert_or_assign(_tool->toolId(), _tool);
	return _tool->toolId();
}

OToolkitAPI::AbstractTool * OToolkitAPI::ToolManager::getTool(ToolID _id) {
	auto it = m_tools.find(_id);
	if (it == m_tools.end()) {
		assert(0); // no tool found
		return nullptr;
	}
	return it->second;
}

OToolkitAPI::AbstractTool * OToolkitAPI::ToolManager::getToolByName(const QString& _name) {
	for (auto t : m_tools) {
		if (t.second->toolName() == _name) { return t.second; };
	}
	assert(0); // Invalid name
	return nullptr;
}

void OToolkitAPI::ToolManager::unloadTool(ToolID _id) {
	AbstractTool * tool = getTool(_id);
	if (tool == nullptr) return;
	delete tool;
	m_tools.erase(_id);
}

void OToolkitAPI::ToolManager::removeToolFromList(ToolID _id) {
	m_tools.erase(_id);
}

// ######################################################################################################################################

OToolkitAPI::ToolManager::ToolManager() : m_currentId(0) {
	
}

OToolkitAPI::ToolManager::~ToolManager() {

}