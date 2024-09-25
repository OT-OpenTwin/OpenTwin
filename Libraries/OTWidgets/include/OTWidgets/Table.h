//! @file Table.h
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/TableCfg.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtWidgets/qtablewidget.h>

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

		void setupFromConfig(const TableCfg& _config);
		TableCfg createConfig(void) const;

		void setContentChanged(bool _changed = true);
		bool getContentChanged(void) const { return m_contentChanged; };

		void setTableName(const std::string& _name) { m_tableName = _name; };
		const std::string& getTableName(void) const { return m_tableName; };

		void setTableTitle(const QString& _title) { m_tableTitle = _title; };
		const QString& getTableTitle(void) const { return m_tableTitle; };

	Q_SIGNALS:
		void saveRequested(void);

		// ###########################################################################################################################################################################################################################################################################################################################

	protected:
		virtual void contentSaved(void) {};
		virtual void contentChanged(void) {};

		// ###########################################################################################################################################################################################################################################################################################################################

	private Q_SLOTS:
		void slotSaveRequested(void);
		void slotCellDataChanged(int _row, int _column);

	private:
		void ini(void);

		bool m_contentChanged;
		std::string m_tableName;
		QString m_tableTitle;

	};

}
