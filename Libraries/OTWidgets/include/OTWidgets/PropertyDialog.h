//! @file PropertyDialog.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/PropertyDialogCfg.h"
#include "OTWidgets/Dialog.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

namespace ot {

	class PropertyGrid;

	class OT_WIDGETS_API_EXPORT PropertyDialog : public Dialog {
		Q_OBJECT
		OT_DECL_NOCOPY(PropertyDialog)
	public:
		PropertyDialog(const PropertyDialogCfg& _config, QWidget* _parentWidget = (QWidget*)nullptr);
		virtual ~PropertyDialog();

	private Q_SLOTS:
		void slotConfirm(void);
		void slotCancel(void);

	private:
		PropertyGrid* m_grid;
		bool m_changed;
	};

}
