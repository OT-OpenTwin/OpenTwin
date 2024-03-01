//! @file ColorPickButton.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Color.h"
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/QWidgetInterface.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtWidgets/qwidget.h>

namespace ot {

	class PushButton;
	class ColorPreviewBox;

	class ColorPickButton : public QWidget, public QWidgetInterface {
		Q_OBJECT
		OT_DECL_NOCOPY(ColorPickButton)
	public:
		ColorPickButton(const QColor& _color = QColor(), QWidget* _parent = (QWidget*)nullptr);
		ColorPickButton(const ot::Color& _color, QWidget* _parent = (QWidget*)nullptr);
		virtual ~ColorPickButton();

		virtual QWidget* getQWidget(void) { return this; };

		void setColor(const ot::Color& _color);
		void setColor(const QColor& _color);
		const QColor& color(void) const;

		void replaceButtonText(const QString& _text);

	Q_SIGNALS:
		void colorChanged(void);

	public Q_SLOTS:
		void slotBrowse(void);

	protected:
		void updateButtonText(void);

	private:
		void ini(const QColor& _color);

		ColorPreviewBox* m_view;
		PushButton* m_btn;
	};

}