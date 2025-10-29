// @otlicense

//! @file EventNotifier.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtCore/qobject.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT EventNotifierMousePressed : public QObject {
		Q_OBJECT
	public:
		EventNotifierMousePressed(QObject* _parent = (QObject*)nullptr);
		virtual bool eventFilter(QObject* _obj, QEvent* _event) override;

	Q_SIGNALS:
		void leftMouseButtonPressed(void);
		void middleMouseButtonPressed(void);
		void rightMouseButtonPressed(void);
	};

}
