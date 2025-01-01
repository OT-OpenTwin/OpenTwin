//! @file MainWindow.h
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtWidgets/qmainwindow.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT MainWindow : public QMainWindow {
		OT_DECL_NOCOPY(MainWindow)
	public:
		MainWindow();
		virtual ~MainWindow();

	private:

	};

}