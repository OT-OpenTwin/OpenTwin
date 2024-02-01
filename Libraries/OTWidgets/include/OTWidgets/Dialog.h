//! @file Dialog.h
//! @author Alexander Kuester (alexk95)
//! @date November 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/OTWidgetsAPIExport.h"
#include "OTWidgets/QWidgetInterface.h"

// Qt header
#include <QtWidgets/qdialog.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT Dialog : public QDialog, public ot::QWidgetInterface {
		OT_DECL_NOCOPY(Dialog)
	public:
		enum DialogResult {
			Ok,
			Yes,
			No,
			Retry,
			Cancel
		};

		Dialog(QWidget* _parent = (QWidget*)nullptr);
		virtual ~Dialog();
		
		//! @brief Returns a pointer to the root widget of this object
		virtual QWidget* getQWidget(void) { return this; }

		//! @brief Center the dialog on parent and call exec
		int showDialog(void);

		void close(DialogResult _result);

		DialogResult dialogResult(void) const { return m_result; };

	private:
		DialogResult m_result;
	};

}