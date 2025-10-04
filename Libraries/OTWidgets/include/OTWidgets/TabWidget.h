//! @file TabWidget.h
//! @author Alexander Kuester (alexk95)
//! @date October 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/OTWidgetsAPIExport.h"
#include "OTWidgets/WidgetBase.h"

// Qt header
#include <QtWidgets/qtabwidget.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT TabWidget : public QTabWidget, public ot::WidgetBase {
		Q_OBJECT
	public:
		TabWidget();
		virtual ~TabWidget();

		virtual void focusInEvent(QFocusEvent* _event) override;
		virtual void focusOutEvent(QFocusEvent* _event) override;

		//! @brief Returns a pointer to the root widget of this object
		virtual QWidget* getQWidget(void) override { return this; };
		virtual const QWidget* getQWidget(void) const override { return this; };

	Q_SIGNALS:
		void widgetGotFocus(void);
		void widgetLostFocus(void);
	};

}
