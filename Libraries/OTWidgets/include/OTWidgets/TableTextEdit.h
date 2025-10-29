// @otlicense

//! @file TableTextEdit.h
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/PlainTextEdit.h"

// Qt header
#include <QtCore/qabstractitemmodel.h>

namespace ot {

	class Table;

	class OT_WIDGETS_API_EXPORT TableTextEdit : public PlainTextEdit {
		Q_OBJECT
		OT_DECL_NOCOPY(TableTextEdit)
		OT_DECL_NOMOVE(TableTextEdit)
		OT_DECL_NODEFAULT(TableTextEdit)
	public:
		TableTextEdit(QWidget* _parent, Table* _table, const QModelIndex& _modelIndex);
		virtual ~TableTextEdit();

		void setModelIndex(const QModelIndex& _index) { m_modelIndex = _index; };
		const QModelIndex& getModelIndex(void) const { return m_modelIndex; };

	protected:
		virtual void keyPressEvent(QKeyEvent* _event) override;
		virtual void keyReleaseEvent(QKeyEvent* _event) override;

	private:
		Table* m_table;
		QModelIndex m_modelIndex;

		bool focusCell(int _row, int _column);
	};

}