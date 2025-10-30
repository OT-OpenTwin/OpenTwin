// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetTypes.h"

// Qt header
#include <QtWidgets/qmainwindow.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT MainWindow : public QMainWindow {
		OT_DECL_NOCOPY(MainWindow)
		OT_DECL_NOMOVE(MainWindow)
	public:
		MainWindow();
		virtual ~MainWindow();

	private:

	};

}