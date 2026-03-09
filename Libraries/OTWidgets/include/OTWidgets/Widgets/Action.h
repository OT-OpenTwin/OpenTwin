// @otlicense
// File: Action.h
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
#include <QtWidgets/qaction.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT Action : public QAction {
		OT_DECL_NOCOPY(Action)
		OT_DECL_NOMOVE(Action)
	public:
		explicit Action(QObject* _parent = (QObject*)nullptr);
		explicit Action(const QString& _text, QObject* _parent = (QObject*)nullptr);
		explicit Action(const QIcon& _icon, const QString& _text, QObject* _parent = (QObject*)nullptr);
		virtual ~Action();
	};

}