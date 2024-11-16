//! @file Table.h
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Color.h"
#include "OTGui/TableCfg.h"
#include "OTGui/TableRange.h"
#include "OTGui/EntityViewBaseInfo.h"
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

		void setupFromConfig(const TableCfg& _config);
		TableCfg createConfig(void) const;

		void setContentChanged(bool _changed = true);
		bool getContentChanged(void) const { return m_contentChanged; };

		void setTableName(const std::string& _name);
		const std::string& getTableName(void) const;

		void setTableTitle(const std::string& _title);
		const std::string& getTableTitle(void) const;

		void setTableEntityId(UID _id);
		UID getTableEntityId(void) const;

		void setTableEntityVersion(UID _version);
		UID getTableEntityVersion(void) const;

		void setSelectedCellsBackground(const ot::Color& _color);
		void setSelectedCellsBackground(const QColor& _color);

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
		EntityViewBaseInfo m_tableInfo;
	};

}
