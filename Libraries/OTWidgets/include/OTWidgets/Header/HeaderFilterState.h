// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetTypes.h"

// Qt header
#include <QtCore/qstring.h>
#include <QtCore/qstringlist.h>

// std header
#include <map>
#include <list>

namespace ot {

	class OT_WIDGETS_API_EXPORT HeaderFilterState
	{
		OT_DECL_DEFCOPY(HeaderFilterState)
		OT_DECL_DEFMOVE(HeaderFilterState)
	public:
		HeaderFilterState() = default;
		virtual ~HeaderFilterState() = default;

		inline void setHoveredIndex(int _index) { m_hoveredFilter = _index; };
		inline int getHoveredIndex() const { return m_hoveredFilter; };

		inline void setPressedIndex(int _index) { m_pressedFilter = _index; };
		inline int getPressedIndex() const { return m_pressedFilter; };

		void setFilter(int _index, const QStringList& _filters) { m_selectedFilters.insert_or_assign(_index, _filters); };
		QStringList getFilter(int _index) const;

		bool isFilterActive(int _index) const;
		std::list<int> getActiveFilters() const;

	private:
		int m_hoveredFilter = -1;
		int m_pressedFilter = -1;
		std::map<int, QStringList> m_selectedFilters;
	};

}