// @otlicense
// File: GraphicsDecoration.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

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
