//! \file WidgetEventNotifier.h
//! \author Alexander Kuester (alexk95)
//! \date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/OTWidgetsAPIExport.h"

class QEvent;
class QWidget;

namespace ot {

	//! \class WidgetEventNotifier
	//! \brief The WidgetEventNotifier can be used to receive event notifications from a WidgetEventHandler.
	class OT_WIDGETS_API_EXPORT WidgetEventNotifier {
	public:
		WidgetEventNotifier();
		virtual ~WidgetEventNotifier();

		virtual void parentWidgetEvent(QEvent* _event) {};

		void setParentWidget(QWidget* _widget) { m_widget; };
		QWidget* getParentWidget(void) const { return m_widget; };

	private:
		QWidget* m_widget;
	};

}