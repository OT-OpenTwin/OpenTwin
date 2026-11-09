// @otlicense
// File: TextEditorMenuRequestEvent.cpp
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

// OpenTwin header
#include "OTGui/Event/TextEditorMenuRequestData.h"
#include "OTWidgets/Event/TextEditorMenuRequestEvent.h"
#include "OTWidgets/Menu/MenuManagerHandler.h"
#include "OTWidgets/WidgetView/WidgetView.h"

ot::TextEditorMenuRequestEvent::TextEditorMenuRequestEvent(MenuCallbackBase* _callbackBase, const QPoint& _pos)
	: MenuRequestWidgetEvent(_callbackBase, _pos)
{
	OTAssertNullptr(getPlainTextEdit());
}

ot::MenuRequestData* ot::TextEditorMenuRequestEvent::createRequestData(const MenuManagerHandler* _handler) const
{
	BasicEntityInformation entityInfo;
	PlainTextEdit* textEdit = this->getPlainTextEdit();
	OTAssertNullptr(textEdit);

	const WidgetView* parentView = textEdit->getParentWidgetView();
	if (parentView)
	{
		entityInfo = parentView->getViewData();
	}

	return new TextEditorMenuRequestData(entityInfo);
}
