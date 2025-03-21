//! @file TableView.h
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetView.h"

namespace ot {

	class Table;

	class OT_WIDGETS_API_EXPORT TableView : public WidgetView {
		Q_OBJECT
		OT_DECL_NOCOPY(TableView)
	public:
		TableView(Table* _table = (Table*)nullptr);
		virtual ~TableView();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Base class functions

		virtual QWidget* getViewWidget(void) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		Table* getTable(void) const { return m_table; };

	private Q_SLOTS:
		void slotModifiedChanged(bool _isModified);

	private:
		Table* m_table;
	};

}