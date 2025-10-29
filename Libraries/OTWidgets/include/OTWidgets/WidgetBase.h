// @otlicense

//! @file WidgetBase.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetTypes.h"

// Qt header
#include <QtWidgets/qwidget.h>

namespace ot {

	class WidgetView;

	class OT_WIDGETS_API_EXPORTONLY WidgetBase {
	public:
		WidgetBase() : m_parentView(nullptr) {};
		virtual ~WidgetBase() {};

		//! @brief Returns a pointer to the root widget of this object
		virtual QWidget* getQWidget() = 0;
		virtual const QWidget* getQWidget() const = 0;

		void setOTWidgetFlags(const WidgetFlags& _flags);
		const WidgetFlags& otWidgetFlags() const { return m_widgetFlags; };

		void setParentWidgetView(WidgetView* _view) { m_parentView = _view; };
		WidgetView* getParentWidgetView() const { return m_parentView; };

		void setSuccessForeground(bool _enabled = true);
		void setWarningForeground(bool _enabled = true);
		void setErrorForeground(bool _enabled = true);

	protected:
		virtual void otWidgetFlagsChanged(const WidgetFlags& _flags) {};

	private:
		WidgetFlags m_widgetFlags;
		WidgetView* m_parentView;
	};
}