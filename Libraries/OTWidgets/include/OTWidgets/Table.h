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
	};

}
