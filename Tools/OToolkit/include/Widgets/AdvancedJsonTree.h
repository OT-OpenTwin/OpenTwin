// @otlicense
// File: AdvancedJsonTree.h
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
#include "OTWidgets/Widgets/WidgetBase.h"

// Qt header
#include <QtCore/qtimer.h>

namespace ot
{
	class Label;
	class LineEdit;
	class JsonDocument;
	class JsonTreeWidget;
	class JsonTreeWidgetNode;

	class AdvancedJsonTree : public QWidget, public WidgetBase
	{
		OT_DECL_NOCOPY(AdvancedJsonTree)
		OT_DECL_NOMOVE(AdvancedJsonTree)
		OT_DECL_NODEFAULT(AdvancedJsonTree)
	public:
		enum class FilterMode
		{
			OnChange,
			AfterDelay
		};

		explicit AdvancedJsonTree(QWidget* _parent);

		virtual QWidget* getQWidget() override { return this; };
		virtual const QWidget* getQWidget() const override { return this; };

		JsonTreeWidget* getTree() { return m_tree; };
		const JsonTreeWidget* getTree() const { return m_tree; };

		void setFromJsonDocument(const JsonDocument& _doc);

		void setFilterMode(FilterMode _mode) { m_status.filterMode = _mode; };
		FilterMode getFilterMode() const { return m_status.filterMode; };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Slots

	public Q_SLOTS:
		void applyCurrentFilter();
		void exportToFile();

	private Q_SLOTS:
		void slotFilterTextChanged();
		void slotNodeDoubleClicked(int _column, JsonTreeWidgetNode* _node);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Private: Helper

	private:
		struct Status
		{
			uint64_t numberOfItems = 0;
			uint64_t numberOfHiddenItems = 0;
			FilterMode filterMode = FilterMode::AfterDelay;
		};

		bool statesDiffer(bool _visualChangesOnly) const;

		void updateStatus();

		JsonTreeWidget* m_tree;
		Label* m_statusLabel;

		LineEdit* m_textFilter;
		QTimer m_filterTimer;

		Status m_status;
		Status m_lastStatus;
	};


}