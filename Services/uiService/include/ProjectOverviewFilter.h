// @otlicense
// File: ProjectOverviewFilter.h
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
#include "ProjectOverviewFilterData.h"
#include "OTWidgets/WidgetTypes.h"

// Qt header
#include <QtCore/qstring.h>
#include <QtCore/qstringlist.h>
#include <QtWidgets/qmenu.h>

class QListWidget;
class QListWidgetItem;

namespace ot {

	class Label;
	class LineEdit;
	class ProjectOverviewWidget;

	class ProjectOverviewFilter : public QMenu {
		Q_OBJECT
		OT_DECL_NOCOPY(ProjectOverviewFilter)
		OT_DECL_NOMOVE(ProjectOverviewFilter)
		OT_DECL_NODEFAULT(ProjectOverviewFilter)
	public:
		ProjectOverviewFilter(ProjectOverviewWidget* _overview, int _logicalIndex, bool _sortOnly);
		virtual ~ProjectOverviewFilter() = default;

		void setTitle(const QString& _title);

		void setOptions(const QStringList& _options);
		ProjectOverviewFilterData getFilterData() const;

		void setFromData(const ProjectOverviewFilterData& _data);

		bool isConfirmed() const { return m_isConfirmed; };

	Q_SIGNALS:
		void filterChanged(const ProjectOverviewFilterData& _filterData);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Private: Slots

	private Q_SLOTS:
		void slotConfirm();
		void slotCancel();
		void slotTextChanged();
		void slotSortAscending();
		void slotSortDescending();
		void slotCheckedChanged(QListWidgetItem* _item);

	private:
		int m_logicalIndex;
		bool m_isConfirmed;

		ProjectOverviewFilterData::SortMode m_sortMode;
		Label* m_title;
		LineEdit* m_filterEdit;
		QListWidget* m_optionsList;
	};

}