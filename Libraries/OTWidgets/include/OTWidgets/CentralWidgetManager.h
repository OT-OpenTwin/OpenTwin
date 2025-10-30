// @otlicense
// File: CentralWidgetManager.h
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
#include "OTWidgets/DelayedShowHideHandler.h"

// Qt header
#include <QtGui/qmovie.h>
#include <QtWidgets/qwidget.h>

class QLabel;

namespace ot {

	class MainWindow;

	class OT_WIDGETS_API_EXPORT CentralWidgetManager : public QWidget {
		OT_DECL_NOCOPY(CentralWidgetManager)
	public:

		//! @brief Constructor.
		//! @param _window If provided the CentralWidgetManager will set itself as the central widget. This may destroy the previously set central widget.
		CentralWidgetManager(MainWindow* _window = (MainWindow*)nullptr);
		virtual ~CentralWidgetManager();

		//! @brief Replaces the current central widget.
		//! An existing widget will be hidden an returned.
		//! Caller takes ownership of the replaced widget.
		std::unique_ptr<QWidget> replaceCurrentCentralWidget(QWidget* _widget);

		//! @brief Returns writeable pointer to the currently set central widget.
		//! The CentralWidgetManager keeps ownership of the central widget.
		QWidget* getCurrentCentralWidget(void) { return m_currentCentral; };

		//! @brief Returns read-only pointer to the currently set central widget.
		//! The CentralWidgetManager keeps ownership of the central widget.
		const QWidget* getCurrentCentralWidget(void) const { return m_currentCentral; };

		void setOverlayAnimation(std::shared_ptr<QMovie> _animation);

		std::shared_ptr<QMovie> getCurrentOverlayAnimation(void) const { return m_animation; };

		void setOverlayAnimationVisible(bool _visible, bool _noDelay = false);
		
		void setOverlayAnimatioShowDelay(int _delay);
		int getOverlayAnimatioShowDelay(void) const;

		void setOverlayAnimatioHideDelay(int _delay);
		int getOverlayAnimatioHideDelay(void) const;

	protected:
		virtual void resizeEvent(QResizeEvent* _event) override;

	private Q_SLOTS:
		void slotShowAnimation(void);
		void slotHideAnimation(void);

	private:
		void applyWaitingAnimationVisible(bool _visible);

		QWidget* m_currentCentral;

		DelayedShowHideHandler m_animationShowHideHandler;
		std::shared_ptr<QMovie> m_animation;
		QLabel* m_animationLabel;
		bool m_animationVisible;
	};

}