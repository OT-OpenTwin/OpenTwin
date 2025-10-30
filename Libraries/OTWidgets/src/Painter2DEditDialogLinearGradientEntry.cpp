// @otlicense
// File: Painter2DEditDialogLinearGradientEntry.cpp
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
#include "OTGui/LinearGradientPainter2D.h"
#include "OTWidgets/DoubleSpinBox.h"
#include "OTWidgets/Painter2DEditDialogGradientBase.h"
#include "OTWidgets/Painter2DEditDialogLinearGradientEntry.h"

// Qt header
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qlayout.h>

ot::Painter2DEditDialogLinearGradientEntry::Painter2DEditDialogLinearGradientEntry(const Painter2D* _painter) {
	const LinearGradientPainter2D* actualPainter = dynamic_cast<const LinearGradientPainter2D*>(_painter);

	m_cLayW = new QWidget;
	m_cLay = new QVBoxLayout(m_cLayW);

	QGridLayout* positionLay = new QGridLayout;

	QLabel* startXLabel = new QLabel("Start X:");
	m_startX = new DoubleSpinBox;
	m_startX->setRange(std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max());
	m_startX->setDecimals(2);
	m_startX->setSuffix("%");
	m_startX->setToolTip("The start X position for the gradient. The painted area is between 0.0 and 1.0. The value 2.0 is equal to double the width and -1.0 the width in negative direction.");
	if (actualPainter) m_startX->setValue(actualPainter->getStart().x() * 100.);
	positionLay->addWidget(startXLabel, 0, 0);
	positionLay->addWidget(m_startX, 0, 1);

	QLabel* startYLabel = new QLabel("Start Y:");
	m_startY = new DoubleSpinBox;
	m_startY->setRange(std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max());
	m_startY->setDecimals(2);
	m_startY->setSuffix("%");
	m_startY->setToolTip("The start Y position for the gradient. The painted area is between 0.0 and 1.0. The value 2.0 is equal to double the height and -1.0 the height in negative direction.");
	if (actualPainter) m_startY->setValue(actualPainter->getStart().y() * 100.);
	positionLay->addWidget(startYLabel, 1, 0);
	positionLay->addWidget(m_startY, 1, 1);

	QLabel* endXLabel = new QLabel("End X:");
	m_finalX = new DoubleSpinBox;
	m_finalX->setRange(std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max());
	m_finalX->setDecimals(2);
	m_finalX->setValue(100.);
	m_finalX->setSuffix("%");
	m_finalX->setToolTip("The fianl stop X position for the gradient. The painted area is between 0.0 and 1.0. The value 2.0 is equal to double the width and -1.0 the width in negative direction.");
	if (actualPainter) m_finalX->setValue(actualPainter->getFinalStop().x() * 100.);
	positionLay->addWidget(endXLabel, 2, 0);
	positionLay->addWidget(m_finalX, 2, 1);

	QLabel* endYLabel = new QLabel("End Y:");
	m_finalY = new DoubleSpinBox;
	m_finalY->setRange(std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max());
	m_finalY->setDecimals(2);
	m_finalY->setValue(100.);
	m_finalX->setSuffix("%");
	m_finalY->setToolTip("The fianl stop Y position for the gradient. The painted area is between 0.0 and 1.0. The value 2.0 is equal to double the height and -1.0 the height in negative direction.");
	if (actualPainter) m_finalY->setValue(actualPainter->getFinalStop().y() * 100.);
	positionLay->addWidget(endYLabel, 3, 0);
	positionLay->addWidget(m_finalY, 3, 1);

	m_cLay->addLayout(positionLay);
	m_gradientBase = new Painter2DEditDialogGradientBase(m_cLay, actualPainter);

	this->connect(m_startX, &DoubleSpinBox::valueChanged, this, &Painter2DEditDialogEntry::slotValueChanged);
	this->connect(m_startY, &DoubleSpinBox::valueChanged, this, &Painter2DEditDialogEntry::slotValueChanged);
	this->connect(m_finalX, &DoubleSpinBox::valueChanged, this, &Painter2DEditDialogEntry::slotValueChanged);
	this->connect(m_finalY, &DoubleSpinBox::valueChanged, this, &Painter2DEditDialogEntry::slotValueChanged);
	this->connect(m_gradientBase, &Painter2DEditDialogGradientBase::valuesChanged, this, &Painter2DEditDialogEntry::slotValueChanged);
}

ot::Painter2DEditDialogLinearGradientEntry::~Painter2DEditDialogLinearGradientEntry() {
	delete m_cLayW;
}

ot::Painter2D* ot::Painter2DEditDialogLinearGradientEntry::createPainter() const {
	ot::LinearGradientPainter2D* newPainter = new ot::LinearGradientPainter2D;
	newPainter->setStart(ot::Point2DD(m_startX->value() / 100., m_startY->value() / 100.));
	newPainter->setFinalStop(ot::Point2DD(m_finalX->value() / 100., m_finalY->value() / 100.));
	newPainter->setStops(m_gradientBase->stops());
	newPainter->setSpread(m_gradientBase->gradientSpread());
	return newPainter;
}

