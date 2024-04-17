//! @file ToolWidgets.h
//! @author Alexander Kuester (alexk95)
//! @date April 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// Toolkit API header
#include "OToolkitAPI/otoolkitapi_global.h"

// Qt header
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qtoolbar.h>

// std header
#include <list>

class QWidget;

namespace otoolkit {

	class OTOOLKITAPI_EXPORT ToolWidgets { 
	public:
		ToolWidgets();
		ToolWidgets(const ToolWidgets& _other);
		virtual ~ToolWidgets() {};

		ToolWidgets& operator = (const ToolWidgets& _other);

		void setRootWidget(QWidget* _widget) { m_widget = _widget; };
		QWidget* rootWidget(void) const { return m_widget; };

		void addStatusWidget(QWidget* _widget) { m_statusWidgets.push_back(_widget); };
		void setStatusWidgets(const std::list<QWidget*>& _statusWidgets) { m_statusWidgets = _statusWidgets; };
		const std::list<QWidget*> statusWidgets(void) const { return m_statusWidgets; };

		void setToolBar(QToolBar* _toolBar) { m_toolBar = _toolBar; };
		QToolBar* toolBar(void) const { return m_toolBar; };
		
	private:
		QWidget* m_widget;
		QToolBar* m_toolBar;
		std::list<QWidget*> m_statusWidgets;
	};

}