//! @file ToolViewManager.h
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"

// Qt header
#include <QtCore/qobject.h>
#include <QtCore/qstring.h>

// std header
#include <map>
#include <list>

class ToolRuntimeHandler;
namespace ot { class WidgetView; };

class ToolViewManager : public QObject{
	Q_OBJECT
	OT_DECL_NOCOPY(ToolViewManager)
public:
	ToolViewManager();
	virtual ~ToolViewManager();

	// ###########################################################################################################################################################################################################################################################################################################################

	// Tool management

	void addTool(ToolRuntimeHandler* _handler);

	void removeTool(const QString& _toolName);

	void updateViews(const QString& _toolName);

	void addIgnoredView(ot::WidgetView* _view) { m_ignoredViews.push_back(_view); };

	// ###########################################################################################################################################################################################################################################################################################################################

	// Setter / Getter / Finder

	ToolRuntimeHandler* findTool(const QString& _toolName) const;
	ToolRuntimeHandler* findTool(ot::WidgetView* _view) const;

Q_SIGNALS:
	void viewFocused(const QString& _viewName, const QString& _toolName);
	void viewFocusLost(const QString& _viewName, const QString& _toolName);
	void viewCloseRequested(const QString& _viewName, const QString& _toolName);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Slots

private Q_SLOTS:
	void slotViewFocused(ot::WidgetView* _view);
	void slotViewFocusLost(ot::WidgetView* _view);
	void slotViewCloseRequested(ot::WidgetView* _view);

private:
	bool isViewIgnored(ot::WidgetView* _view) const;

	std::map<QString, ToolRuntimeHandler*> m_nameMap;
	std::map<ot::WidgetView*, ToolRuntimeHandler*> m_viewMap;
	std::list<ot::WidgetView*> m_ignoredViews;
	
};