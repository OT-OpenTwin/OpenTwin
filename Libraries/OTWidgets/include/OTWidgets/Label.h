//! @file Label.h
//! @author Alexander Kuester (alexk95)
//! @date April 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/OTWidgetsAPIExport.h"
#include "OTWidgets/QWidgetInterface.h"

// Qt header
#include <QtWidgets/qlabel.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT Label : public QLabel, public ot::QWidgetInterface {
		Q_OBJECT
	public:
		Label(QWidget* _parentWidget = (QWidget*)nullptr);
		Label(const QString& _text, QWidget* _parentWidget = (QWidget*)nullptr);
		virtual ~Label() {};

		//! @brief Returns a pointer to the root widget of this object
		virtual QWidget* getQWidget(void) override { return this; };

		virtual void mousePressEvent(QMouseEvent* _event) override;

	Q_SIGNALS:
		void mousePressed(void);
	};
}