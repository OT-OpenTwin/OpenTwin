//! @file Table.h
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Color.h"
#include "OTGui/TableCfg.h"
#include "OTGui/TableRange.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtWidgets/qtablewidget.h>

// std header
#include <vector>

namespace ot {

	class OT_WIDGETS_API_EXPORT Table : public QTableWidget {
		Q_OBJECT
		OT_DECL_NOCOPY(Table)
	public:
		Table(QWidget* _parentWidget = (QWidget*)nullptr);
		Table(int _rows, int _columns, QWidget* _parentWidget = (QWidget*)nullptr);
		virtual ~Table();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		virtual void setupFromConfig(const TableCfg& _config);
		virtual TableCfg createConfig(void) const;

		void setContentChanged(bool _changed = true);
		bool getContentChanged(void) const { return m_contentChanged; };

		void setSelectedCellsBackground(const ot::Color& _color);
		void setSelectedCellsBackground(const QColor& _color);

		void prepareForDataChange(void);

	Q_SIGNALS:
		void saveRequested(void);
		void modifiedChanged(bool _isModified);

	public Q_SLOTS:
		void slotSaveRequested(void);

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
		void ini(void);
		void resizeColumnsToContentIfNeeded(void);
		void resizeRowsToContentIfNeeded(void);
		void setResizeRequired(void);

		bool m_stopResizing;
		bool m_resizeRequired;
		bool m_contentChanged;
		std::vector<int> m_columnWidthBuffer;
		std::vector<int> m_rowHeightBuffer;
	};

}
