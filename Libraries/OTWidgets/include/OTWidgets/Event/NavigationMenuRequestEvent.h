// @otlicense
// File: NavigationMenuRequestEvent.h
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
#include "OTWidgets/Event/MenuRequestWidgetEvent.h"

// Qt header
#include <QtWidgets/qtreewidget.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT NavigationMenuRequestEvent : public MenuRequestWidgetEvent
	{
		OT_DECL_NOCOPY(NavigationMenuRequestEvent)
		OT_DECL_NOMOVE(NavigationMenuRequestEvent)
		OT_DECL_NODEFAULT(NavigationMenuRequestEvent)
	public:
		NavigationMenuRequestEvent(MenuCallbackBase* _callbackBase, const QPoint& _pos);
		virtual ~NavigationMenuRequestEvent() = default;
		
		//! @brief Create a new instance of the menu request data for this event. 
		//! Caller takes ownership of the created instance.
		OT_DECL_NODISCARD virtual MenuRequestData* createRequestData(const MenuManagerHandler* _handler) const override;

		OT_DECL_NODISCARD QTreeWidget* getTreeWidget() const { return this->getWidgetAs<QTreeWidget>(); };

		OT_DECL_NODISCARD QTreeWidgetItem* getItemAt(const QPoint& _pos) const;
		OT_DECL_NODISCARD QTreeWidgetItem* getItem() const { return getItemAt(this->getPosition()); };

	};
} // namespace ot