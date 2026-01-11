// @otlicense
// File: GraphicsDecoration.cpp
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

// OpenTwin header
#include "OTCore/Logging/LogDispatcher.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/GraphicsDecoration.h"

ot::GraphicsDecoration::GraphicsDecoration() {

}

ot::GraphicsDecoration::~GraphicsDecoration() {

}

void ot::GraphicsDecoration::paintDecoration(QPainter* _painter, const QPointF& _position, qreal _angle) const {
	if (m_config.getSymbol() == GraphicsDecorationCfg::NoSymbol) {
		return;
	}

	_painter->save();

	_painter->translate(_position.x(), _position.y());
	_painter->rotate(_angle);
	
	if (m_config.getReverse()) {
		_painter->rotate(180.);
	}

	_painter->setPen(QtFactory::toQPen(m_config.getOutlinePen()));
	QPainterPath decorationPath = this->createDecorationPath();
	_painter->drawPath(decorationPath);

	if (this->isDecorationFilled()) {
		_painter->fillPath(decorationPath, QtFactory::toQBrush(m_config.getFillPainter()));
	}

	_painter->restore();
}

QPainterPath ot::GraphicsDecoration::createDecorationPath() const {
	switch (m_config.getSymbol()) {
	case GraphicsDecorationCfg::NoSymbol: return QPainterPath();
	case GraphicsDecorationCfg::Triangle:
	case GraphicsDecorationCfg::FilledTriange:
		return this->createTrianglePath();

	case GraphicsDecorationCfg::Arrow: return this->createArrowPath();
	case GraphicsDecorationCfg::VArrow: return this->createVArrowPath();

	case GraphicsDecorationCfg::Circle:
	case GraphicsDecorationCfg::FilledCircle:
		return this->createCirclePath();
			
	case GraphicsDecorationCfg::Square:
	case GraphicsDecorationCfg::FilledSquare:
		return this->createSquarePath();

	case GraphicsDecorationCfg::Diamond:
	case GraphicsDecorationCfg::FilledDiamond:
		return this->createDiamondPath();

	case GraphicsDecorationCfg::Cross: return this->createCrossPath();
	default:
		OT_LOG_E("Unknown symbol (" + std::to_string(static_cast<int>(m_config.getSymbol())) + ")");
		return QPainterPath();
	}
}

bool ot::GraphicsDecoration::isDecorationFilled() const {
	switch (m_config.getSymbol()) {
	case GraphicsDecorationCfg::FilledTriange:
	case GraphicsDecorationCfg::FilledCircle:
	case GraphicsDecorationCfg::FilledSquare:
	case GraphicsDecorationCfg::FilledDiamond:
	case GraphicsDecorationCfg::Arrow:
		return true;

	default:
		return false;
	}
}

QPainterPath ot::GraphicsDecoration::createTrianglePath() const {
	const double w = m_config.getSize().width();
	const double h = m_config.getSize().height();

	QPainterPath path;
	path.moveTo(-(w / 2.), 0.);          // tip (left)
	path.lineTo(w / 2., -(h / 2.));      // top-right
	path.lineTo(w / 2., (h / 2.));       // bottom-right
	path.lineTo(-(w / 2.), 0.);
	return path;
}

QPainterPath ot::GraphicsDecoration::createArrowPath() const {
	const double w = m_config.getSize().width();
	const double h = m_config.getSize().height();

	QPainterPath path;
	path.moveTo(-(w / 2.), 0.);         // tip
	path.lineTo(w / 2., -(h / 2.));     // top-back
	path.lineTo(w / 6., 0);             // inner notch
	path.lineTo(w / 2., h / 2.);        // top-bottom
	path.lineTo(-(w / 2.), 0.);
	return path;
}

QPainterPath ot::GraphicsDecoration::createVArrowPath() const {
	const double w = m_config.getSize().width();
	const double h = m_config.getSize().height();

	QPainterPath path;
	path.moveTo(w / 2., -(h / 2.));      // top back
	path.lineTo(-(w / 2.), 0.);          // tip
	path.lineTo(w / 2., (h / 2.));       // bottom back
	return path;
}

QPainterPath ot::GraphicsDecoration::createCirclePath() const {
	const double w = m_config.getSize().width();
	const double h = m_config.getSize().height();

	QPainterPath path;
	path.addEllipse(QPointF(0., 0.), w / 2., h / 2.);
	return path;
}

QPainterPath ot::GraphicsDecoration::createSquarePath() const {
	const double w = m_config.getSize().width();
	const double h = m_config.getSize().height();

	QPainterPath path;
	path.moveTo(-(w / 2.), (h / 2.));
	path.lineTo((w / 2.), (h / 2.));
	path.lineTo((w / 2.), -(h / 2.));
	path.lineTo(-(w / 2.), -(h / 2.));
	path.lineTo(-(w / 2.), (h / 2.));
	return path;
}

QPainterPath ot::GraphicsDecoration::createDiamondPath() const {
	const double w = m_config.getSize().width();
	const double h = m_config.getSize().height();

	QPainterPath path;
	path.moveTo(-(w / 2.), 0.);         // left tip
	path.lineTo(0., -(h / 2.));         // top
	path.lineTo((w / 2.), 0.);          // right tip
	path.lineTo(0., (h / 2.));          // bottom
	path.lineTo(-(w / 2.), 0.);
	return path;
}

QPainterPath ot::GraphicsDecoration::createCrossPath() const {
	const double w = m_config.getSize().width();
	const double h = m_config.getSize().height();

	QPainterPath path;
	path.moveTo(-(w / 2.), -(h / 2.));
	path.lineTo((w / 2.), (h / 2.));
	path.moveTo((w / 2.), -(h / 2.));
	path.lineTo(-(w / 2.), (h / 2.));
	return path;
}
