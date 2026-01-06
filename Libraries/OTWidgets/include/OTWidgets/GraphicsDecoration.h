// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/Point2D.h"
#include "OTGui/Graphics/GraphicsDecorationCfg.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtGui/qpainter.h>
#include <QtGui/qpainterpath.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT GraphicsDecoration {
		OT_DECL_NOCOPY(GraphicsDecoration)
		OT_DECL_NOMOVE(GraphicsDecoration)
	public:
		GraphicsDecoration();
		virtual ~GraphicsDecoration();

		void setConfiguration(const GraphicsDecorationCfg& _cfg) { m_config = _cfg; };
		const GraphicsDecorationCfg& getConfiguration() const { return m_config; };

		void paintDecoration(QPainter* _painter, const QPointF& _position, qreal _angle) const;

		QPainterPath createDecorationPath() const;

	protected:
		bool isDecorationFilled() const;

	private:
		GraphicsDecorationCfg m_config;

		QPainterPath createTrianglePath() const;
		QPainterPath createArrowPath() const;
		QPainterPath createVArrowPath() const;
		QPainterPath createCirclePath() const;
		QPainterPath createSquarePath() const;
		QPainterPath createDiamondPath() const;
		QPainterPath createCrossPath() const;

	};

}
