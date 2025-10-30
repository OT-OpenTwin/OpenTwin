// @otlicense
// File: aLabelWidget.h
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
#include <qlabel.h>					// Base class
#include <qstring.h>				// QString

// AK header
#include <akWidgets/aWidget.h>
#include <akCore/globalDataTypes.h>

namespace ak {

	class UICORE_API_EXPORT aLabelWidget : public QLabel, public aWidget
	{
		Q_OBJECT
	public:
		aLabelWidget(QWidget * _parent = nullptr);

		aLabelWidget(const QString & _text, QWidget * _parent = nullptr);

		virtual ~aLabelWidget();

		// #############################################################################################################################
		// Base class functions

		//! @brief Will return the widgets widget to display it
		virtual QWidget * widget(void) override;

	Q_SIGNALS:
		void clicked();

	protected:
		bool event(QEvent *myEvent);

	private:
		aLabelWidget(const aLabelWidget &) = delete;
		aLabelWidget & operator = (const aLabelWidget &) = delete;
	};
} // namespace ak
