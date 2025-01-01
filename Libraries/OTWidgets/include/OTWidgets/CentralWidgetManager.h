//! @file CentralWidgetManager.h
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Flags.h"
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtGui/qmovie.h>
#include <QtWidgets/qwidget.h>

class QTimer;
class QLabel;

namespace ot {

	class OT_WIDGETS_API_EXPORT CentralWidgetManager : public QWidget {
		OT_DECL_NOCOPY(CentralWidgetManager)
	public:
		CentralWidgetManager();
		virtual ~CentralWidgetManager();

		//! @brief Replaces the current central widget.
		//! An existing widget will be hidden an returned.
		//! Caller takes ownership of the replaced widget.
		QWidget* replaceCurrentCentralWidget(QWidget* _widget);

		QMovie* replaceWaitingAnimation(QMovie* _animation);
		QMovie* getWaitingAnimation(void) const;

		void setWaitingAnimationVisible(bool _visible, bool _noDelay = false);
		
		void setShowDelay(int _delay) { m_showDelay = _delay; };
		int getShowDelay(void) const { return m_showDelay; };

		void setHideDelay(int _delay) { m_hideDelay = _delay; };
		int getHideDelay(void) const { return m_hideDelay; };

	protected:
		virtual void resizeEvent(QResizeEvent* _event) override;

	private Q_SLOTS:
		void slotTimeout(void);

	private:
		void applyWaitingAnimationVisible(bool _visible);

		enum class AnimationState {
			HiddenState = 0 << 0,
			DelayedShow = 1 << 0,
			DelayedHide = 1 << 1,
			IsVisible   = 1 << 2
		};
		OT_ADD_FRIEND_FLAG_FUNCTIONS(AnimationState)
		typedef Flags<AnimationState> AnimationFlags;

		QWidget* m_currentCentral;

		QTimer* m_delayTimer;
		int m_showDelay;
		int m_hideDelay;

		QLabel* m_animationLabel;
		AnimationFlags m_animationFlags;

	};

}