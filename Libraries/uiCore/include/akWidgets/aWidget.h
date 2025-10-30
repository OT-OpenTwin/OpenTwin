// @otlicense
// File: aWidget.h
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

// AK header
#include <akCore/globalDataTypes.h>
#include <akCore/akCore.h>
#include <akCore/aObject.h>

// Forward declaration
class QWidget;

namespace ak {

	//! This class provides a interface that represents a widget.
	//! Every class derived from this class must be able to provide a QWidget that is representing it
	class UICORE_API_EXPORT aWidget : public aObject {
	public:
		//! @brief Constructor
		//! @param _type Object type
		//! @param _UID The initial UID of this object
		aWidget(
			objectType			_type = otNone,
			UID					_UID = invalidUID
		);

		//! @brief Deconstructor
		virtual ~aWidget();

		//! @brief Will return the widgets widget to display it
		virtual QWidget * widget(void) = 0;

		//! @brief Returns true the object is derived from aWidget
		virtual bool isWidgetType(void) const override;

	private:
		// Block copy constructor
		aWidget(const aWidget & _other) = delete;

		// Block assignment operator
		aWidget & operator = (const aWidget & _other) = delete;
	};
} // namespace ak