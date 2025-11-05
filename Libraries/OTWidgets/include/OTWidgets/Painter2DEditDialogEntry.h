// @otlicense
// File: Painter2DEditDialogEntry.h
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

namespace ot {

	class Painter2D;

	class OT_WIDGETS_API_EXPORT Painter2DEditDialogEntry : public QObject {
		Q_OBJECT
	public:
		enum DialogEntryType {
			FillType,
			LinearType,
			RadialType,
			ReferenceType
		};

		Painter2DEditDialogEntry() {};
		virtual ~Painter2DEditDialogEntry();

		virtual DialogEntryType getEntryType() const = 0;
		virtual QWidget* getRootWidget() const = 0;

		//! @brief Creates a new painter.
		//! Caller takes ownership.
		virtual Painter2D* createPainter() const = 0;

	Q_SIGNALS:
		void valueChanged();

	public Q_SLOTS:
		void slotValueChanged();
	};

}