// @otlicense
// File: ProjectOverviewHeader.h
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
#include <QtWidgets/qheaderview.h>

// std header
#include <map>

namespace ot {

	class ProjectOverviewWidget;

	class ProjectOverviewHeader : public QHeaderView {
		Q_OBJECT
		OT_DECL_NOCOPY(ProjectOverviewHeader)
		OT_DECL_NOMOVE(ProjectOverviewHeader)
		OT_DECL_NODEFAULT(ProjectOverviewHeader)
	public:
		enum ColumnIndex {
			Checked,
			Group,
			Type,
			Name,
			Tags,
			Owner,
			Access,
			LastAccessed,
			Count
		};

		ProjectOverviewHeader(ProjectOverviewWidget* _overview, QWidget* _parent);
		virtual ~ProjectOverviewHeader();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Overrides

		virtual int sizeHintForColumn(int _column) const override;

		void setFilterData(const ProjectFilterData& _filterData);

	protected:
		virtual void paintSection(QPainter* _painter, const QRect& _rect, int _logicalIndex) const override;
		virtual QSize sectionSizeFromContents(int _logicalIndex) const override;
		virtual void mousePressEvent(QMouseEvent* _event) override;
		virtual void mouseReleaseEvent(QMouseEvent* _event) override;
		virtual void mouseMoveEvent(QMouseEvent* _event) override;
		virtual void leaveEvent(QEvent* _event) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Private: Slots

	private Q_SLOTS:
		void slotSortChanged(int _logicalIndex, Qt::SortOrder _sortOrder);
		
		// ###########################################################################################################################################################################################################################################################################################################################

		// Private: Helper

	private:
		bool canFilter(int _logicalIndex) const;
		QRect filterIconRect(int _logicalIndex) const;
		void showFilterMenu(int _logicalIndex);

		ProjectOverviewWidget* m_overview;

		std::map<int, QStringList> m_filterOptions;
		ProjectOverviewFilterData m_lastFilter;

		const QSize c_buttonSize;
		const QSize c_buttonPadding;
		int m_hoveredFilter;
		int m_pressedFilter;
		int m_activeFilter;
	};

}