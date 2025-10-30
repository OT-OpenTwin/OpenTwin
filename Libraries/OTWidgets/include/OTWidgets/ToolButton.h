// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetBase.h"

// Qt header
#include <QtWidgets/qtoolbutton.h>

namespace ot {

	class Action;

	class OT_WIDGETS_API_EXPORT ToolButton : public QToolButton, public WidgetBase {
		Q_OBJECT
		OT_DECL_NOCOPY(ToolButton)
	public:
		ToolButton(QWidget* _parent = (QWidget*)nullptr);
		ToolButton(const QString& _text, QWidget* _parent = (QWidget*)nullptr);
		ToolButton(const QIcon& _icon, const QString& _text, QWidget* _parent = (QWidget*)nullptr);

		virtual ~ToolButton();

		// #######################################################################################################

		// Base class functions

		//! @brief Will return the widgets widget to display it
		virtual QWidget* getQWidget(void) override { return this; };
		virtual const QWidget* getQWidget(void) const override { return this; };

		// #######################################################################################################

	private:
		//! @brief Initializes the components of this toolButton
		void ini(void);
		
		Action* m_action;
	};
}
