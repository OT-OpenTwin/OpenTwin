//! @file ColorPickButton.h
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
	class ColorPreviewBox;

	class OT_WIDGETS_API_EXPORT ColorPickButton : public QFrame, public QWidgetInterface {
		Q_OBJECT
		OT_DECL_NOCOPY(ColorPickButton)
	public:
		ColorPickButton(const QColor& _color = QColor(), QWidget* _parent = (QWidget*)nullptr);
		ColorPickButton(const ot::Color& _color, QWidget* _parent = (QWidget*)nullptr);
		virtual ~ColorPickButton();

		virtual QWidget* getQWidget(void) override { return this; };

		void setColor(const ot::Color& _color);
		void setColor(const QColor& _color);
		const QColor& color(void) const;
		ot::Color otColor(void) const;

		void useCustomToolTip(bool _use = true);
		bool isUseCustomToolTip(void) const { return m_useCustomToolTip; };

		void useAlpha(bool _use = true);
		bool isUseAlpha(void) const { return m_useAlpha; };

		void replaceButtonText(const QString& _text);

	Q_SIGNALS:
		void colorChanged(void);

	public Q_SLOTS:
		void slotBrowse(void);

	protected:
		void updateButtonText(void);

	private:
		void ini(const QColor& _color);

		bool m_useCustomToolTip;
		bool m_useAlpha;
		ColorPreviewBox* m_view;
		PushButton* m_btn;
	};

}