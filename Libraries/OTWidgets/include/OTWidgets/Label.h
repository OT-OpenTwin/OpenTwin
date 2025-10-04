//! @file Label.h
//! @author Alexander Kuester (alexk95)
//! @date April 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/OTWidgetsAPIExport.h"
#include "OTWidgets/WidgetBase.h"

// Qt header
#include <QtWidgets/qlabel.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT Label : public QLabel, public ot::WidgetBase {
		Q_OBJECT
	public:
		Label(QWidget* _parentWidget = (QWidget*)nullptr);
		Label(const QString& _text, QWidget* _parentWidget = (QWidget*)nullptr);
		virtual ~Label() {};

		//! @brief Returns a pointer to the root widget of this object
		virtual QWidget* getQWidget() override { return this; };
		virtual const QWidget* getQWidget() const override { return this; };

	Q_SIGNALS:
		void mouseClicked();
		void mouseDoubleClicked();

	protected:
		virtual void mousePressEvent(QMouseEvent* _event) override;
		virtual void mouseReleaseEvent(QMouseEvent* _event) override;
		virtual void mouseDoubleClickEvent(QMouseEvent* _event) override;

	private:
		bool m_mouseIsPressed;
	};
}