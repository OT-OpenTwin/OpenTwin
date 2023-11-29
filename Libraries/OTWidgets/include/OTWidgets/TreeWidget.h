//! @file TreeWidget.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/OTWidgetsAPIExport.h"
#include "OTWidgets/QWidgetInterface.h"

// Qt header
#include <QtWidgets/qtreewidget.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT TreeWidget : public QTreeWidget, public ot::QWidgetInterface {
	public:
		TreeWidget(QWidget * _parentWidget = (QWidget*)nullptr);
		virtual ~TreeWidget();

		//! @brief Returns a pointer to the root widget of this object
		virtual QWidget* getQWidget(void) override { return this; };

	private:

	};
}