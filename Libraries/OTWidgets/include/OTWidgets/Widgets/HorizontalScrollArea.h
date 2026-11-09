// @otlicense
// File: HorizontalScrollArea.h
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
#include "OTWidgets/Widgets/ScrollArea.h"

namespace ot {

	class OT_WIDGETS_API_EXPORT HorizontalScrollArea : public ScrollArea
	{
		Q_OBJECT
		OT_DECL_NOCOPY(HorizontalScrollArea)
		OT_DECL_NOMOVE(HorizontalScrollArea)
		OT_DECL_NODEFAULT(HorizontalScrollArea)
	public:
		explicit HorizontalScrollArea(QWidget* _parent);
		virtual ~HorizontalScrollArea() = default;

		virtual bool eventFilter(QObject* _watched, QEvent* _event) override;
	};

}