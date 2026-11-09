// @otlicense
// File: SDSServiceEntry.h
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
#include "OTWidgets/Widgets/TreeWidget.h"
#include "OTWidgets/Widgets/OnOffSlider.h"
#include "OTWidgets/Widgets/TreeWidgetItem.h"

// Qt header
#include <QtCore/qobject.h>

class SDSServiceEntry : public QObject, public ot::TreeWidgetItem {
	Q_OBJECT
	OT_DECL_NOCOPY(SDSServiceEntry)
	OT_DECL_NOMOVE(SDSServiceEntry)
	OT_DECL_NODEFAULT(SDSServiceEntry)
public:
	enum class ColumnIndex : int {
		ServiceName,
		ToggleButton,
		ColumnCount
	};

	SDSServiceEntry(ot::TreeWidget* _tree, const std::string& _lssUrl, const std::string& _serviceName, bool _isDebug);

	ot::OnOffSlider* getToggleSlider() const { return m_toggleSlider; };

public Q_SLOTS:
	void toggle();

private Q_SLOTS:
	void debugToggled(bool _checked);

private:
	std::string m_lssUrl;
	std::string m_name;
	bool m_isDebug;
	
	ot::OnOffSlider* m_toggleSlider;

};