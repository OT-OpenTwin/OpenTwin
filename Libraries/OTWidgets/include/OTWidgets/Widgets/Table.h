// @otlicense
// File: Table.h
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
#include "OTCore/Color.h"
#include "OTGui/TableRange.h"
#include "OTGui/Widgets/TableCfg.h"
#include "OTWidgets/Widgets/WidgetBase.h"

// Qt header
#include <QtWidgets/qtablewidget.h>

// std header
#include <vector>

class QTableWidgetItem;

namespace ot {

	class TableItem;
	class TableHeader;
	class TableItemDelegate;

	class OT_WIDGETS_API_EXPORT Table : public QTableWidget, public WidgetBase {
		Q_OBJECT
		OT_DECL_NOCOPY(Table)
		OT_DECL_NOMOVE(Table)
		OT_DECL_NODEFAULT(Table)
	public:
		static QRect getSelectionBoundingRect(const QList<QTableWidgetSelectionRange>& _selections);

		explicit Table(QWidget* _parentWidget);
		explicit Table(int _rows, int _columns, QWidget* _parentWidget);
		virtual ~Table();

		virtual QWidget* getQWidget() override { return this; };
		virtual const QWidget* getQWidget() const override { return this; };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		virtual void setupFromConfig(const TableCfg& _config);
		virtual TableCfg createConfig() const;

		void setContentChanged(bool _changed = true);
		bool getContentChanged() const { return m_contentChanged; };

		void setSelectedCellsBackground(const ot::Color& _color);
		void setSelectedCellsBackground(const QColor& _color);

		void prepareForDataChange();

		QRect getSelectionBoundingRect() const { return Table::getSelectionBoundingRect(this->selectedRanges()); };

		//! @brief Enables multiline handling for cells.
		//! This must be called before filling the table since it affects the text edit created by the item delegate.
		void setMultilineCells(bool _multiline = true) { m_multilineCells = _multiline; };
		bool getMultilineCells() const { return m_multilineCells; };

		//! @brief Creates a default item and palaces it at the given coordinates.
		//! @return Default item. The table keeps ownership of the item.
		TableItem* addItem(int _row, int _column, const QString& _text, const QString& _sortHint = QString());

		//! @brief  Creates a default item and palaces it at the given coordinates.
		//! @return Default item. The table keeps ownership of the item.
		TableItem* addItem(int _row, int _column, const QIcon& _icon, const QString& _text, const QString& _sortHint = QString());

		const TableHeaderItemCfg* getHorizontalHeaderItemCfg(int _column) const;
		const TableHeaderItemCfg* getVerticalHeaderItemCfg(int _row) const;

		bool getFilterRowSortingEnabled() const { return m_filterRowSortingEnabled; };
		bool getFilterColumnSortingEnabled() const { return m_filterColumnSortingEnabled; };

	Q_SIGNALS:
		void saveRequested();
		void modifiedChanged(bool _isModified);

	public Q_SLOTS:
		void slotSaveRequested();
		
		// ###########################################################################################################################################################################################################################################################################################################################

	protected:
		virtual void showEvent(QShowEvent* _event) override;
		virtual void hideEvent(QHideEvent* _event) override;

		// ###########################################################################################################################################################################################################################################################################################################################

	private Q_SLOTS:
		void slotCellDataChanged(int _row, int _column);
		void slotRestoreColumnSize(int _column);
		void slotResizeColumnToContent(int _column);
		void slotRestoreRowSize(int _row);
		void slotResizeRowToContent(int _row);
		
	private:
		void ini();
		void resizeColumnsToContentIfNeeded();
		void resizeRowsToContentIfNeeded();
		void setResizeRequired();
		void clearHeaderConfigs();
		QTableWidgetItem* createHeaderItem(const TableHeaderItemCfg* _cfg) const;

		TableItemDelegate* m_itemDelegate;

		bool m_multilineCells;
		bool m_stopResizing;
		bool m_resizeRequired;
		bool m_contentChanged;
		std::vector<int> m_columnWidthBuffer;
		std::vector<int> m_rowHeightBuffer;

		bool m_filterRowSortingEnabled = false;
		bool m_filterColumnSortingEnabled = false;

		TableHeader* m_horizontalHeader;
		std::vector<TableHeaderItemCfg*> m_horizontalHeaderItemCfgs;

		TableHeader* m_verticalHeader;
		std::vector<TableHeaderItemCfg*> m_verticalHeaderItemCfgs;
	};

}
