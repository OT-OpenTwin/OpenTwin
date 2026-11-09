// @otlicense
// File: TextEditorMenuRequestEvent.h
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
#include "OTWidgets/Widgets/PlainTextEdit.h"
#include "OTWidgets/Event/MenuRequestWidgetEvent.h"

namespace ot {

	class OT_WIDGETS_API_EXPORT TextEditorMenuRequestEvent : public MenuRequestWidgetEvent
	{
		OT_DECL_NOCOPY(TextEditorMenuRequestEvent)
		OT_DECL_NOMOVE(TextEditorMenuRequestEvent)
		OT_DECL_NODEFAULT(TextEditorMenuRequestEvent)
	public:
		TextEditorMenuRequestEvent(MenuCallbackBase* _callbackBase, const QPoint& _pos);
		virtual ~TextEditorMenuRequestEvent() = default;

		//! @brief Create a new instance of the menu request data for this event. 
		//! Caller takes ownership of the created instance.
		virtual MenuRequestData* createRequestData(const MenuManagerHandler* _handler) const override;

		OT_DECL_NODISCARD PlainTextEdit* getPlainTextEdit() const { return this->getWidgetAs<PlainTextEdit>(); };

	};
} // namespace ot