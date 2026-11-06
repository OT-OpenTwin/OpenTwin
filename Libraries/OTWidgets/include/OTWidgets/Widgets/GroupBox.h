// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/Widgets/WidgetBase.h"

// Qt header
#include <QtWidgets/qgroupbox.h>

namespace ot
{

	class OT_WIDGETS_API_EXPORT GroupBox : public QGroupBox, public WidgetBase
	{
		Q_OBJECT
		OT_DECL_NOCOPY(GroupBox)
		OT_DECL_NOMOVE(GroupBox)
	public:
		GroupBox(QWidget* _parent = nullptr);
		GroupBox(const QString& _title, QWidget* _parent = nullptr);
		virtual ~GroupBox() = default;

		virtual QWidget* getQWidget() override { return this; };
		virtual const QWidget* getQWidget() const override { return this; };
	};

}