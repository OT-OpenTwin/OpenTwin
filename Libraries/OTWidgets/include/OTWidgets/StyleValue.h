//! @file StyleValue.h
//! @author Alexander Kuester (alexk95)
//! @date November 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtCore/qstring.h>
#include <QtGui/qcolor.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT StyleValue {
	public:
		StyleValue();
		virtual ~StyleValue();

		void setQss(const QString& _qss) { m_qss = _qss; };
		QString qss(void) const { return m_qss; };

		void setColor(const QColor& _color) { m_color = _color; };
		QColor color(void) const { return m_color; };

	private:
		QString m_qss;
		QColor m_color;

	};

}
