// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/Widgets/WidgetBase.h"

// Qt header
#include <QtWidgets/qprogressbar.h>

namespace ot
{
	class OT_WIDGETS_API_EXPORT ProgressBar : public QProgressBar, public WidgetBase
	{
		Q_OBJECT
		OT_DECL_NOCOPY(ProgressBar)
		OT_DECL_NOMOVE(ProgressBar)
		OT_DECL_NODEFAULT(ProgressBar)
	public:
		ProgressBar(QWidget* _parent);
		virtual ~ProgressBar();

		virtual QWidget* getQWidget() override { return this; };
		virtual const QWidget* getQWidget() const override { return this; };

	};
}
