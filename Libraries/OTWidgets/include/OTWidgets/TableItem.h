// @otlicense
// File: TableItem.h
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