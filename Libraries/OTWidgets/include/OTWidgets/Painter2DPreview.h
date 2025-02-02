//! @file Painter2DPreview.h
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/QWidgetInterface.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtWidgets/qframe.h>

namespace ot {

	class Painter2D;

	class OT_WIDGETS_API_EXPORT Painter2DPreview : public QFrame, public QWidgetInterface {
		Q_OBJECT
		OT_DECL_NOCOPY(Painter2DPreview)
	public:
		Painter2DPreview(const Painter2D* _painter = (Painter2D*) nullptr);
		virtual ~Painter2DPreview();

		void setFromPainter(const ot::Painter2D* _painter);

		virtual void paintEvent(QPaintEvent* _event) override;

		virtual QWidget* getQWidget(void) override { return this; };
		virtual const QWidget* getQWidget(void) const override { return this; };

		void setMaintainAspectRatio(bool _maintain) { m_maintainAspectRatio = _maintain; };
		bool isMaintainAspectRatio(void) const { return m_maintainAspectRatio; };

	private Q_SLOTS:
		void slotGlobalStyleChanged(void);

	private:
		bool m_maintainAspectRatio;
		QBrush m_brush;
	};

}