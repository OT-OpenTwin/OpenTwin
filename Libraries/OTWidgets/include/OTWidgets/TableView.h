//! @file TableView.h
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/Table.h"
#include "OTWidgets/WidgetView.h"

namespace ot {

	class OT_WIDGETS_API_EXPORT TableView : public Table, public WidgetView {
		OT_DECL_NOCOPY(TableView)
	public:
		TableView();
		virtual ~TableView();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Base class functions

		virtual QWidget* getViewWidget(void) override;

		virtual void setupFromConfig(const TableCfg& _config) override;
		virtual TableCfg createConfig(void) const override;

		// ###########################################################################################################################################################################################################################################################################################################################

	protected:
		virtual void contentSaved(void) override;
		virtual void contentChanged(void) override;

	};

}