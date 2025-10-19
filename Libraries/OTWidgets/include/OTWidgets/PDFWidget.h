//! @file PDFWidget.h
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetBase.h"

namespace ot {

	class OT_WIDGETS_API_EXPORT PDFWidget : public ot::WidgetBase {
		Q_OBJECT
		OT_DECL_NOCOPY(PDFWidget)
		OT_DECL_NOMOVE(PDFWidget)
	public:
		PDFWidget(QWidget* _parentWidget = (QWidget*)nullptr);
		virtual ~PDFWidget();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Virtual methods

		virtual QWidget* getQWidget() override { return m_rootWidget; };
		virtual const QWidget* getQWidget() const override { return m_rootWidget; };

	private:
		QWidget* m_rootWidget;
	};

}