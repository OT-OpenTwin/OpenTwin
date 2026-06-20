// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/Widgets/WidgetBase.h"

// Qt header
#include <QtWidgets/qstatusbar.h>

namespace ot
{
	class OT_WIDGETS_API_EXPORT StatusBar : public QStatusBar, public WidgetBase
	{
		Q_OBJECT
		OT_DECL_NOCOPY(StatusBar)
		OT_DECL_NOMOVE(StatusBar)
		OT_DECL_NODEFAULT(StatusBar)
	public:
		StatusBar(QWidget* _parent);
		virtual ~StatusBar();
		
		virtual QWidget* getQWidget() override { return this; };
		virtual const QWidget* getQWidget() const override { return this; };

	};
}