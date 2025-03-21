//! @file Action.h
//! @author Alexander Kuester (alexk95)
//! @date September 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtWidgets/qaction.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT Action : public QAction {
		OT_DECL_NOCOPY(Action)
	public:
		Action(QObject* _parent = (QObject*)nullptr);
		Action(const QString& _text, QObject* _parent = (QObject*)nullptr);
		Action(const QIcon& _icon, const QString& _text, QObject* _parent = (QObject*)nullptr);
		virtual ~Action();
	};

}