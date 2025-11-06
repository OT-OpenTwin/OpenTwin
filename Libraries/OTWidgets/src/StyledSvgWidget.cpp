// @otlicense
// File: StyledSvgWidget.cpp
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
#include "OTWidgets/IconManager.h"
#include "OTWidgets/StyledSvgWidget.h"
#include "OTWidgets/SvgImagePainter.h"
#include "OTWidgets/GlobalColorStyle.h"

// Qt header
#include <QtGui/qpainter.h>

ot::StyledSvgWidget::StyledSvgWidget(const QString& _fileSubPath, QWidget* _parent)
	: QFrame(_parent), m_fileSubPath(_fileSubPath), m_maintainAspectRatio(true), m_svgPainter(nullptr)
{
	this->setFrameShape(QFrame::NoFrame);

	m_svgPainter = new SvgImagePainter(IconManager::getParsedSvgData(_fileSubPath));
	connect(&GlobalColorStyle::instance(), &GlobalColorStyle::currentStyleChanged, this, &ot::StyledSvgWidget::slotStyleChanged);
}

ot::StyledSvgWidget::~StyledSvgWidget() {
	disconnect(&GlobalColorStyle::instance(), &GlobalColorStyle::currentStyleChanged, this, &ot::StyledSvgWidget::slotStyleChanged);
	if (m_svgPainter) {
		delete m_svgPainter;
		m_svgPainter = nullptr;
	}
}

QSize ot::StyledSvgWidget::sizeHint() const {
	QSize hint;
	if (m_svgPainter) {
		hint = m_svgPainter->getDefaultImageSize();
	}
	return hint.expandedTo(minimumSize()).boundedTo(maximumSize());
}

void ot::StyledSvgWidget::paintEvent(QPaintEvent* _event) {
	//QFrame::paintEvent(_event);

	if (m_svgPainter) {
		QPainter painter(this);
		m_svgPainter->paintImage(&painter, this->rect().marginsRemoved(QMargins(1, 1, 1, 1)), m_maintainAspectRatio);
	}
}

void ot::StyledSvgWidget::slotStyleChanged() {
	if (m_svgPainter) {
		delete m_svgPainter;
		m_svgPainter = nullptr;
	}
	m_svgPainter = new SvgImagePainter(IconManager::getParsedSvgData(m_fileSubPath));
	update();
}