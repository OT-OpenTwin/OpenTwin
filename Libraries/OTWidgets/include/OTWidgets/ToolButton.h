//! @file ToolButton.h
//! @author Alexander Kuester (alexk95)
//! @date September 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/QWidgetInterface.h"

// Qt header
#include <QtWidgets/qtoolbutton.h>

namespace ot {

	class Action;

	class OT_WIDGETS_API_EXPORT ToolButton : public QToolButton, public QWidgetInterface {
		Q_OBJECT
		OT_DECL_NOCOPY(ToolButton)
	public:
		ToolButton();
		ToolButton(const QString& _text);
		ToolButton(const QIcon& _icon, const QString& _text);

		virtual ~ToolButton();

		// #######################################################################################################

		// Base class functions

		//! @brief Will return the widgets widget to display it
		virtual QWidget* getQWidget(void) override { return this; }

		// #######################################################################################################

	private:
		//! @brief Initializes the components of this toolButton
		void ini(void);
		
		Action* m_action;
	};
}
