// @otlicense
// File: SvgImagePainter.cpp
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
#include "OTCore/LogDispatcher.h"
#include "OTWidgets/SvgImagePainter.h"

// Qt header
#include <QtGui/qpainter.h>
#include <QtSvg/qsvgrenderer.h>

ot::SvgImagePainter::SvgImagePainter(const QByteArray& _svgData) :
	m_svgRenderer(nullptr), m_svgData(_svgData)
{
	m_svgRenderer = new QSvgRenderer(_svgData);
	OTAssertNullptr(m_svgRenderer);
}

ot::SvgImagePainter::SvgImagePainter(const SvgImagePainter& _other) 
	: ImagePainter(_other), m_svgRenderer(nullptr), m_svgData(_other.m_svgData)
{
	m_svgRenderer = new QSvgRenderer(m_svgData);
	OTAssertNullptr(m_svgRenderer);
}

ot::SvgImagePainter::~SvgImagePainter() {
	OTAssertNullptr(m_svgRenderer);
	delete m_svgRenderer;
	m_svgRenderer = nullptr;
}

void ot::SvgImagePainter::paintImage(QPainter* _painter, const QRect& _bounds, bool _maintainAspectRatio) const {
	OTAssertNullptr(m_svgRenderer);
	m_svgRenderer->render(_painter, _bounds.toRectF());
}

void ot::SvgImagePainter::paintImage(QPainter* _painter, const QRectF& _bounds, bool _maintainAspectRatio) const {
	OTAssertNullptr(m_svgRenderer);
	m_svgRenderer->render(_painter, _bounds);
}

QSize ot::SvgImagePainter::getDefaultImageSize() const {
	OTAssertNullptr(m_svgRenderer);
	return m_svgRenderer->defaultSize();
}

QSizeF ot::SvgImagePainter::getDefaultImageSizeF(void) const {
	OTAssertNullptr(m_svgRenderer);
	return m_svgRenderer->defaultSize().toSizeF();
}

bool ot::SvgImagePainter::isValid() const {
	OTAssertNullptr(m_svgRenderer);
	return m_svgRenderer->isValid();
}
