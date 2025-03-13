//! @file NavigationTreeView.h
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/CopyInformation.h"
#include "OTWidgets/WidgetView.h"

namespace ak { class aTreeWidget; };

namespace ot {

	class NavigationTreeView : public WidgetView {
		Q_OBJECT
	public:
		NavigationTreeView();
		virtual ~NavigationTreeView();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Base class functions

		virtual QWidget* getViewWidget(void) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		ak::aTreeWidget* getTree(void) const { return m_tree; };

	Q_SIGNALS:
		void copyRequested(const ot::CopyInformation& _info);
		void pasteRequested(const ot::CopyInformation& _info);

	private Q_SLOTS:
		void slotCopyRequested(void);
		void slotPasteRequested(void);

	private:
		ak::aTreeWidget* m_tree;
	};

}
