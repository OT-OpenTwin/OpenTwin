// @otlicense

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
