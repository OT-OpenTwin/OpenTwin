//! @file SVGWidget.h
//! @author Alexander Kuester (alexk95)
//! @date February 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/QWidgetInterface.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtSvgWidgets/qsvgwidget.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT SVGWidget : public QSvgWidget, public QWidgetInterface {
		Q_OBJECT
	public:
		SVGWidget(const QString& _imagePath, QWidget* parent = (QWidget*)nullptr);

		virtual QWidget* getQWidget(void) override { return this; };

		virtual const QWidget* getQWidget(void) const override { return this; };

	private:

	};

}