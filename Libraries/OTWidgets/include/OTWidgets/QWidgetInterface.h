//! @file QWidgetInterface.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Flags.h"
#include "OTWidgets/WidgetTypes.h"

// Qt widget
class QWidget;

namespace ot {

	class QWidgetInterface {
	public:
		QWidgetInterface() {};
		virtual ~QWidgetInterface() {};

		//! @brief Returns a pointer to the root widget of this object
		virtual QWidget* getWidget(void) = 0;

		void setWidgetFlags(const WidgetFlags& _flags) { if (m_widgetFlags.update(_flags)) { widgetFlagsChanged(m_widgetFlags); }; };
		const WidgetFlags& widgetFlags(void) const { return m_widgetFlags; };
		virtual void widgetFlagsChanged(const WidgetFlags& _flags) {};

	protected:
		WidgetFlags m_widgetFlags;

	};
}