// @otlicense
// File: GraphicsConnectionPreviewItem.h
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
#include "OTGui/GraphicsItemCfg.h"
#include "OTGui/GraphicsConnectionCfg.h"
#include "OTWidgets/CustomGraphicsItem.h"

// Qt header
#include <QtGui/qpen.h>
#include <QtCore/qrect.h>

namespace ot {

	class GraphicsItem;

	class OT_WIDGETS_API_EXPORT GraphicsConnectionPreviewItem : public QGraphicsItem {
		OT_DECL_NOCOPY(GraphicsConnectionPreviewItem)
		OT_DECL_NOMOVE(GraphicsConnectionPreviewItem)
	public:
		GraphicsConnectionPreviewItem();
		virtual ~GraphicsConnectionPreviewItem();

		// ###########################################################################################################################################################################################################################################################################################################################

		// QGraphicsItem

		virtual QRectF boundingRect(void) const override;
		virtual void paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		void setConnectionShape(ot::GraphicsConnectionCfg::ConnectionShape _shape);
		ot::GraphicsConnectionCfg::ConnectionShape getConnectionShape(void) const { return m_shape; };

		void setOriginPos(const QPointF& _origin);
		QPointF originPos(void) const { return m_origin; };

		void setOriginDir(ot::ConnectionDirection _direction);
		ot::ConnectionDirection originDir(void) const { return m_originDir; };

		void setDestPos(const QPointF& _dest);
		QPointF destPos(void) const { return m_dest; };

		void setDestDir(ot::ConnectionDirection _direction);
		ot::ConnectionDirection destDir(void) const { return m_originDir; };

	private:
		void calculateSmoothLinePoints(QPointF& _control1, QPointF& _control2) const;
		void calculateSmoothLineStep(const QPointF& _origin, const QPointF& _destination, double _halfdistX, double _halfdistY, QPointF& _control, ot::ConnectionDirection _direction) const;

		const double c_lineWidth;

		QPointF m_origin;
		ot::ConnectionDirection m_originDir;
		QPointF m_dest;
		ot::ConnectionDirection m_destDir;

		ot::GraphicsConnectionCfg::ConnectionShape m_shape;
	};

}