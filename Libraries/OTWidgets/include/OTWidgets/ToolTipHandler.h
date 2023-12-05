//! @file ToolTipHandler.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtCore/qobject.h>
#include <QtCore/qtimer.h>
#include <QtCore/qpoint.h>
#include <QtCore/qstring.h>

namespace ot {

	class ToolTipHandler : public QObject {
		Q_OBJECT
		OT_DECL_NOCOPY(ToolTipHandler)
	public:
		static void showToolTip(const QPoint& _pos, const QString& _text, int _timeout = 0);

		static void hideToolTip(void);

	private slots:
		void slotShowDelayedToolTip(void);

	private:
		static ToolTipHandler& instance(void);
		void showToolTipImpl(const QPoint& _pos, const QString& _text, int _timeout);
		void hideToolTipImpl(void);

		ToolTipHandler();
		~ToolTipHandler();

		QTimer m_timer;
		QPoint m_pos;
		QString m_text;
	};

}