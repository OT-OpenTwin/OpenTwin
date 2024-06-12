//! @file QWidgetInterface.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Flags.h"
#include "OTWidgets/WidgetTypes.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtWidgets/qwidget.h>

namespace ot {

	class OT_WIDGETS_API_EXPORTONLY QWidgetInterface {
	public:
		QWidgetInterface() {};
		virtual ~QWidgetInterface() {};

		//! @brief Returns a pointer to the root widget of this object
		virtual QWidget* getQWidget(void) = 0;

		void setOTWidgetFlags(const WidgetFlags& _flags);
		const WidgetFlags& otWidgetFlags(void) const { return m_widgetFlags; };
		virtual void otWidgetFlagsChanged(const WidgetFlags& _flags) {};

		//! \brief Centers this widget on the parent.
		//! If no parent is provided the widget will center on the screen.
		void centerOnParent(const QWidget* const _parentWidget);

		//! \brief Calculates the top left corner of this widget centered on the parent widget.
		//! If no parent is provided the widget will center on the screen.
		QPoint calculateCenterOnParentPos(const QWidget* const _parentWidget);

	protected:
		WidgetFlags m_widgetFlags;

	};
}