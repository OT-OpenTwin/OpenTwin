// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetTypes.h"

// Qt header
#include <QtCore/qstring.h>
#include <QtCore/qstringlist.h>

namespace ot {

	class ProjectOverviewFilterData {
		OT_DECL_DEFCOPY(ProjectOverviewFilterData)	
		OT_DECL_DEFMOVE(ProjectOverviewFilterData)
	public:		
		ProjectOverviewFilterData(int _logicalIndex = -1);
		~ProjectOverviewFilterData() = default;

		bool isValid() const { return !m_selectedFilters.empty() && m_logicalIndex >= 0; };

		void setLogicalIndex(int _logicalIndex) { m_logicalIndex = _logicalIndex; };
		int getLogicalIndex() const { return m_logicalIndex; };

		void addSelectedFilter(const QString& _filter) { m_selectedFilters.append(_filter); };
		void setSelectedFilters(const QStringList& _filters) { m_selectedFilters = _filters; };
		const QStringList& getSelectedFilters() const { return m_selectedFilters; };

		static QString getEmptyGroupFilterName() { return "< No Groups >"; };
		static QString getEmptyTagsFilterName() { return "< No Tags >"; };

	private:
		QStringList m_selectedFilters;
		int m_logicalIndex;

	};

}