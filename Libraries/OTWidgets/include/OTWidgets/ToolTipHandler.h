// @otlicense
// File: ToolTipHandler.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetTypes.h"

// Qt header
#include <QtCore/qobject.h>
#include <QtCore/qtimer.h>
#include <QtCore/qpoint.h>
#include <QtCore/qstring.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT ToolTipHandler : public QObject {
		Q_OBJECT
		OT_DECL_NOCOPY(ToolTipHandler)
	public:
		static void showToolTip(const QPoint& _pos, const QString& _text, int _timeout = 0);
		//! @brief Shows a tooltip for around specified widget at the specified alignment position.
		//! @param _widget Widget to show the tooltip for.
		//! @param _text Text to show in the tooltip.
		//! @param _alignment Alignment position relative to the widget where the tooltip should be shown.
		//! @param _timeout Timeout in milliseconds after which the tooltip will be automatically hidden. If 0 is specified, the tooltip will remain visible until manually hidden.
		static void showToolTip(const QWidget* _widget, const QString& _text, Alignment _alignment = Alignment::BottomLeft, int _timeout = 0);

		static void hideToolTip(void);

	private Q_SLOTS:
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
