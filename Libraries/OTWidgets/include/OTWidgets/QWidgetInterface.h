//! @file QWidgetInterface.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Flags.h"
#include "OTWidgets/WidgetTypes.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt widget
class QWidget;

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

		void centerOnParent(const QWidget* _parentWidget);

	protected:
		WidgetFlags m_widgetFlags;

	};
}