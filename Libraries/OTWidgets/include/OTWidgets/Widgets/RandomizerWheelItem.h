// @otlicense
// File: RandomizerWheelItem.h
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
#include <QtCore/qvariant.h>

namespace ot
{
	class OT_WIDGETS_API_EXPORT RandomizerWheelItem
	{
		OT_DECL_NOCOPY(RandomizerWheelItem)
		OT_DECL_NOMOVE(RandomizerWheelItem)
		OT_DECL_NODEFAULT(RandomizerWheelItem)
	public:
		explicit RandomizerWheelItem(const QString& _text, const QVariant& _data = QVariant());
		virtual ~RandomizerWheelItem() = default;

		void setText(const QString& _text) { m_text = _text; };
		const QString& getText() const { return m_text; };

		void setData(const QVariant& _data) { m_data = _data; };
		const QVariant& getData() const { return m_data; };

	private:
		QString m_text;
		QVariant m_data;
	};
}