// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/Header/HeaderBase.h"

namespace ot {

	class Table;

	class OT_WIDGETS_API_EXPORT TableHeader : public HeaderBase
	{
		OT_DECL_NOCOPY(TableHeader)
		OT_DECL_NOMOVE(TableHeader)
		OT_DECL_NODEFAULT(TableHeader)
	public:
		explicit TableHeader(Table* _table, Qt::Orientation _orientation);
		virtual ~TableHeader() = default;

	protected:
		virtual HeaderFilter::Features getFilterFeatures(int _logicalIndex) const override;
		virtual QString getFilterTitle(int _logicalIndex) const override;
		virtual QStringList getFilterOptions(int _logicalIndex) const override;

		virtual void sortOrderChangeRequest(int _logicalIndex, Qt::SortOrder _sortOrder) override;
		virtual void filterOptionsChanged(int _logicalIndex, const QStringList& _selectedOptions) override;

		QStringList getFilterOptionsFromText(int _logicalIndex) const;

	private:
		Table* m_table;
	};
}