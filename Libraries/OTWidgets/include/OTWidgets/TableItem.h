// @otlicense

//! @file TableItem.h
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtGui/qicon.h>
#include <QtWidgets/qtablewidget.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT TableItem : public QTableWidgetItem {
	public:
		TableItem() = default;
		TableItem(const QString& _text, const QString& _sortHint = QString());
		TableItem(const QIcon& _icon, const QString& _text, const QString& _sortHint = QString());
		TableItem(const TableItem&) = default;
		TableItem& operator = (const TableItem&) = default;
		TableItem(TableItem&&) = default;
		TableItem& operator = (TableItem&&) = default;

		virtual bool operator < (const QTableWidgetItem& _other) const override;

		//! @brief Set the table item sort hint.
		//! The hint is used when sorting the table.
		//! If the hint is not empty it will be used during comparison otherwise the text will be used.
		void setSortHint(const QString& _hint) { m_sortHint = _hint; };

		//! @brief Return the table item sort hint.
		//! @brief setSortHint
		const QString& getSortHint(void) const { return m_sortHint; };

	private:
		QString m_sortHint;
	};

}