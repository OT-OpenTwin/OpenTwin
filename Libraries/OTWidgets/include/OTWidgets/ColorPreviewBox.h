//! @file ColorPreviewBox.h
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Color.h"
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/QWidgetInterface.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtWidgets/qframe.h>

namespace ot {

	class PushButton;

	class ColorPreviewBox : public QFrame, public QWidgetInterface {
		OT_DECL_NOCOPY(ColorPreviewBox)
	public:
		ColorPreviewBox(const QColor& _color = QColor(), QWidget* _parent = (QWidget*)nullptr);
		ColorPreviewBox(const ot::Color& _color, QWidget* _parent = (QWidget*)nullptr);
		virtual ~ColorPreviewBox();

		virtual QWidget* getQWidget(void) override { return this; };

		virtual QSize sizeHint(void) const override { return QSize(16, 16); };

		void setColor(const ot::Color& _color);
		void setColor(const QColor& _color);
		const QColor& color(void) const { return m_color; };

	protected:
		virtual void paintEvent(QPaintEvent* _event) override;

	private:
		QColor m_color;
	};

}