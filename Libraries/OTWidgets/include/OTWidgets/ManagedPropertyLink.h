// @otlicense
// File: ManagedPropertyLink.h
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

	class WidgetBase;
	class WidgetPropertyManager;

	//! @class ManagedPropertyLink
	//! @brief The ManagedPropertyLink is a base class to link widget components to managed properties.
	//! @ref PropertyManager
	class OT_WIDGETS_API_EXPORT ManagedPropertyLink : public QObject {
		Q_OBJECT
		OT_DECL_NOCOPY(ManagedPropertyLink)
		OT_DECL_NOMOVE(ManagedPropertyLink)
		OT_DECL_NODEFAULT(ManagedPropertyLink)
	public:
		ManagedPropertyLink(WidgetPropertyManager* _manager);
		virtual ~ManagedPropertyLink();

		WidgetPropertyManager* getWidgetPropertyManager(void) { return m_manager; };
		const WidgetPropertyManager* getWidgetPropertyManager(void) const { return m_manager; };

	private:
		WidgetPropertyManager* m_manager;
	};

}