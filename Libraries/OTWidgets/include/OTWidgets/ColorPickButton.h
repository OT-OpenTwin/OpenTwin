// @otlicense

//! @file ColorPickButton.h
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Color.h"
#include "OTWidgets/WidgetBase.h"

// Qt header
#include <QtWidgets/qframe.h>

namespace ot {

	class PushButton;
	class ColorPreviewBox;

	class OT_WIDGETS_API_EXPORT ColorPickButton : public QFrame, public WidgetBase {
		Q_OBJECT
		OT_DECL_NOCOPY(ColorPickButton)
	public:
		ColorPickButton(const QColor& _color = QColor(), QWidget* _parent = (QWidget*)nullptr);
		ColorPickButton(const ot::Color& _color, QWidget* _parent = (QWidget*)nullptr);
		virtual ~ColorPickButton();

		virtual QWidget* getQWidget(void) override { return this; };
		virtual const QWidget* getQWidget(void) const override { return this; };

		void setColor(const ot::Color& _color);
		void setColor(const QColor& _color);
		const QColor& color(void) const;
		ot::Color otColor(void) const;

		void useCustomToolTip(bool _use = true);
		bool isUseCustomToolTip(void) const { return m_useCustomToolTip; };

		void setEditAlpha(bool _use = true);
		bool getEditAlpha(void) const { return m_editAlpha; };

		void replaceButtonText(const QString& _text);

		ColorPreviewBox* getPreviewBox(void) const { return m_view; };
		PushButton* getPushButton(void) const { return m_btn; };

	Q_SIGNALS:
		void colorChanged(void);

	public Q_SLOTS:
		void slotBrowse(void);

	protected:
		void updateButtonText(void);

	private:
		void ini(const QColor& _color);

		bool m_useCustomToolTip;
		bool m_editAlpha;
		ColorPreviewBox* m_view;
		PushButton* m_btn;
	};

}