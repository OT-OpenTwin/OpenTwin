//! @file Painter2DPreview.h
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/ColorStyle.h"
#include "OTWidgets/QWidgetInterface.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtWidgets/qwidget.h>

namespace ot {

	class Painter2D;

	class OT_WIDGETS_API_EXPORT Painter2DPreview : public QWidget, public QWidgetInterface {
		Q_OBJECT
		OT_DECL_NOCOPY(Painter2DPreview)
	public:
		Painter2DPreview(const Painter2D* _painter = (Painter2D*) nullptr);
		virtual ~Painter2DPreview();

		void setFromPainter(const ot::Painter2D* _painter);

		virtual void paintEvent(QPaintEvent* _event) override;

		virtual QWidget* getQWidget(void) override { return this; };

	private Q_SLOTS:
		void slotGlobalStyleChanged(const ColorStyle& _style);

	private:
		QBrush m_brush;
	};

}