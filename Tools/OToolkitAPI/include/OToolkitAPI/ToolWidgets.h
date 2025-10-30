// @otlicense

#pragma once

// Toolkit API header
#include "OToolkitAPI/otoolkitapi_global.h"

// Qt header
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qtoolbar.h>

// std header
#include <list>

class QWidget;

namespace ot { class WidgetView; };

namespace otoolkit {

	class OTOOLKITAPI_EXPORT ToolWidgets { 
	public:
		ToolWidgets();
		ToolWidgets(const ToolWidgets& _other);
		virtual ~ToolWidgets() {};

		ToolWidgets& operator = (const ToolWidgets& _other);

		void addView(ot::WidgetView* _view) { m_views.push_back(_view); };
		void setViews(const std::list<ot::WidgetView*>& _views) { m_views = _views; };
		const std::list<ot::WidgetView*>& getViews(void) const { return m_views; };

		void addStatusWidget(QWidget* _widget) { m_statusWidgets.push_back(_widget); };
		void setStatusWidgets(const std::list<QWidget*>& _statusWidgets) { m_statusWidgets = _statusWidgets; };
		const std::list<QWidget*> getStatusWidgets(void) const { return m_statusWidgets; };

		void setToolBar(QToolBar* _toolBar) { m_toolBar = _toolBar; };
		QToolBar* getToolBar(void) const { return m_toolBar; };
		
	private:
		QToolBar* m_toolBar;
		std::list<ot::WidgetView*> m_views;
		std::list<QWidget*> m_statusWidgets;
	};

}