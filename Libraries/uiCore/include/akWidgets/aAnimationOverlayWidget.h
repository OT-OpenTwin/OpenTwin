// @otlicense
// File: aAnimationOverlayWidget.h
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

// uiCore header
#include <akCore/globalDataTypes.h>

// Qt header
#include <qwidget.h>
#include <qsize.h>

class QLabel;
class QMovie;
class QResizeEvent;

namespace ak {

	class aTimer;

	class UICORE_API_EXPORT aAnimationOverlayWidget : public QWidget {
		Q_OBJECT
	public:
		aAnimationOverlayWidget();
		virtual ~aAnimationOverlayWidget();

		void setChild(
			QWidget *		_widget
		);

		void setWaitingAnimationVisible(
			bool			_visible
		);

		void setWaitingAnimation(
			QMovie *							_movie
		);

		void setAnimationDelay(int _ms) { m_animationDelay =_ms; }

		QLabel* waitingLabel(void) const { return m_waitingLabel; };

	private Q_SLOTS:
		void slotTimeout(void);

	private:

		virtual void resizeEvent(QResizeEvent * _event) override;

		QLabel *			m_waitingLabel;
		QWidget *			m_childWidget;
		int					m_animationDelay;
		bool				m_animationDelayRequested;
		aTimer *			m_delayTimer;
		bool				m_waitingAnimationVisible;

		aAnimationOverlayWidget(aAnimationOverlayWidget&) = delete;
		aAnimationOverlayWidget& operator = (aAnimationOverlayWidget&) = delete;
	};

}
