// @otlicense
// File: aCheckBoxWidget.h
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

// Qt header
#include <qcheckbox.h>				// base class
#include <qstring.h>				// QString

// AK header
#include <akWidgets/aWidget.h>
#include <akCore/globalDataTypes.h>

// Forward declaration
class QKeyEvent;

namespace ak {

	//! @brief This class combines the functionallity of a QCheckBox and a ak::object
	class UICORE_API_EXPORT aCheckBoxWidget : public QCheckBox, public aWidget
	{
		Q_OBJECT
		OT_DECL_NOCOPY(aCheckBoxWidget)
		OT_DECL_NOMOVE(aCheckBoxWidget)
		OT_DECL_NODEFAULT(aCheckBoxWidget)
	public:
		//! @brief Constructor
		//! @param _parent The parent QWidget for this CheckBox
		aCheckBoxWidget(
			QWidget *								_parent
		);

		//! @brief Constructor
		//! @param _text The initial text of the CheckBox
		//! @param _parent The parent QWidget for this CheckBox
		aCheckBoxWidget(
			const QString &							_text,
			QWidget *								_parent
		);

		//! @brief Deconstructor
		virtual ~aCheckBoxWidget();

		// #######################################################################################################

		// Event handling

		//! @brief Emits a key pressend signal a key is pressed
		virtual void keyPressEvent(
			QKeyEvent *								_event
		) override;

		//! @brief Emits a key released signal a key is released
		virtual void keyReleaseEvent(
			QKeyEvent *								_event
		) override;

		// #######################################################################################################

		// Base class functions

		//! @brief Will return the widgets widget to display it
		virtual QWidget * widget(void) override;

		// #######################################################################################################

		void setPaintBackground(bool _paint);

	Q_SIGNALS:
		void keyPressed(QKeyEvent *);
		void keyReleased(QKeyEvent *);

	private:
		bool	m_paintBackground;
	};
} // namespace ak
