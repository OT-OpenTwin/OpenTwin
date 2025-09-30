//! @file ToolViewManager.h
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OToolkit header
#include "ToolViewManager.h"
#include "ToolRuntimeHandler.h"

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTWidgets/WidgetViewManager.h"

ToolViewManager::ToolViewManager() {
	this->connect(&ot::WidgetViewManager::instance(), &ot::WidgetViewManager::viewFocusChanged, this, &ToolViewManager::slotViewFocusChanged);
	this->connect(&ot::WidgetViewManager::instance(), &ot::WidgetViewManager::viewCloseRequested, this, &ToolViewManager::slotViewCloseRequested);
}

ToolViewManager::~ToolViewManager() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Tool management

void ToolViewManager::addTool(ToolRuntimeHandler* _handler) {
	const auto& it = m_nameMap.find(_handler->getTool()->getToolName());
	if (it != m_nameMap.end()) {
		OT_LOG_E("Tool \"" + _handler->getTool()->getToolName().toStdString() + "\" already exists.");
		return;
	}

	m_nameMap.insert_or_assign(_handler->getTool()->getToolName(), _handler);
}

void ToolViewManager::removeTool(const QString& _toolName) {
	ot::WidgetViewManager::instance().closeViews(ot::BasicServiceInformation(_toolName.toStdString()));
}

void ToolViewManager::updateViews(const QString& _toolName) {
	const auto& it = m_nameMap.find(_toolName);
	if (it == m_nameMap.end()) {
		OT_LOG_E("Tool \"" + _toolName.toStdString() + "\" not found.");
		return;
	}

	// Go trough all views and check if they already exist
	for (ot::WidgetView* view : it->second->getToolWidgets().getViews()) {
		OTAssertNullptr(view);
		const auto viewIt = m_viewMap.find(view);
		if (viewIt == m_viewMap.end()) {
			// Store view
			m_viewMap.insert_or_assign(view, it->second);

			// Display view
			ot::WidgetViewManager::instance().addView(ot::BasicServiceInformation(_toolName.toStdString()), view);
		}
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter / Finder

ToolRuntimeHandler* ToolViewManager::findTool(const QString& _toolName) const {
	const auto& it = m_nameMap.find(_toolName);
	if (it == m_nameMap.end()) {
		return nullptr;
	}
	else {
		return it->second;
	}
}

ToolRuntimeHandler* ToolViewManager::findTool(ot::WidgetView* _view) const {
	const auto& it = m_viewMap.find(_view);
	if (it == m_viewMap.end()) {
		return nullptr;
	}
	else {
		return it->second;
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Slots

void ToolViewManager::slotViewFocusChanged(ot::WidgetView* _focused, ot::WidgetView* _previousView) {
	if (_previousView) {
		if (this->isViewIgnored(_previousView)) return;

		const auto& it = m_viewMap.find(_previousView);
		if (it == m_viewMap.end()) {
			OT_LOG_E("View not found");
			return;
		}

		OTAssertNullptr(it->second);
		OTAssertNullptr(it->second->getTool());

		Q_EMIT viewFocusLost(QString::fromStdString(_previousView->getViewData().getEntityName()), it->second->getTool()->getToolName());
	}

	if (_focused) {
		if (this->isViewIgnored(_focused)) return;

		const auto& it = m_viewMap.find(_focused);
		if (it == m_viewMap.end()) {
			OT_LOG_E("View not found");
			return;
		}

		OTAssertNullptr(it->second);
		OTAssertNullptr(it->second->getTool());

		Q_EMIT viewFocused(QString::fromStdString(_focused->getViewData().getEntityName()), it->second->getTool()->getToolName());
	}
}

void ToolViewManager::slotViewCloseRequested(ot::WidgetView* _view) {
	OTAssertNullptr(_view);

	if (this->isViewIgnored(_view)) return;

	const auto& it = m_viewMap.find(_view);
	if (it == m_viewMap.end()) {
		OT_LOG_E("View not found");
		return;
	}

	OTAssertNullptr(it->second);
	OTAssertNullptr(it->second->getTool());

	Q_EMIT viewCloseRequested(QString::fromStdString(_view->getViewData().getEntityName()), it->second->getTool()->getToolName());
}

bool ToolViewManager::isViewIgnored(ot::WidgetView* _view) const {
	return std::find(m_ignoredViews.begin(), m_ignoredViews.end(), _view) != m_ignoredViews.end();
}