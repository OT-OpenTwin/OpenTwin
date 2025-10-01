//! @file Table.h
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Color.h"
#include "OTGui/TableCfg.h"
#include "OTGui/TableRange.h"
#include "OTWidgets/QWidgetInterface.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtWidgets/qtablewidget.h>

// std header
#include <vector>

namespace ot {

	class TableItem;
	class TableItemDelegate;

	class OT_WIDGETS_API_EXPORT Table : public QTableWidget, public QWidgetInterface {
		Q_OBJECT
		OT_DECL_NOCOPY(Table)
	public:
		static QRect getSelectionBoundingRect(const QList<QTableWidgetSelectionRange>& _selections);

		Table(QWidget* _parentWidget = (QWidget*)nullptr);
		Table(int _rows, int _columns, QWidget* _parentWidget = (QWidget*)nullptr);
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

		TableItemDelegate* m_itemDelegate;

		bool m_multilineCells;
		bool m_stopResizing;
		bool m_resizeRequired;
		bool m_contentChanged;
		std::vector<int> m_columnWidthBuffer;
		std::vector<int> m_rowHeightBuffer;

		std::vector<std::string> m_headerBuffer; //The displayed strings are not correct, they don't correspond with the text file. This is because the displayed headers are freed of "-characters
	};

}
