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

	class WidgetView;

	class OT_WIDGETS_API_EXPORTONLY QWidgetInterface {
	public:
		QWidgetInterface() : m_parentView(nullptr) {};
		virtual ~QWidgetInterface() {};

		//! @brief Returns a pointer to the root widget of this object
		virtual QWidget* getQWidget(void) = 0;
		virtual const QWidget* getQWidget(void) const = 0;

		void setOTWidgetFlags(const WidgetFlags& _flags);
		const WidgetFlags& otWidgetFlags(void) const { return m_widgetFlags; };

		void setParentWidgetView(WidgetView* _view) { m_parentView = _view; };
		WidgetView* getParentWidgetView(void) const { return m_parentView; };

	protected:
		virtual void otWidgetFlagsChanged(const WidgetFlags& _flags) {};

	private:
		WidgetFlags m_widgetFlags;
		WidgetView* m_parentView;
	};
}