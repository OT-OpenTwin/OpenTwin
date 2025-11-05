// @otlicense
// File: Painter2DEditDialogRadialGradientEntry.cpp
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
#include "OTGui/RadialGradientPainter2D.h"
#include "OTWidgets/CheckBox.h"
#include "OTWidgets/Label.h"
#include "OTWidgets/DoubleSpinBox.h"
#include "OTWidgets/Painter2DEditDialogGradientBase.h"
#include "OTWidgets/Painter2DEditDialogRadialGradientEntry.h"

// Qt header
#include <QtWidgets/qlayout.h>

ot::Painter2DEditDialogRadialGradientEntry::Painter2DEditDialogRadialGradientEntry(const Painter2D* _painter, QWidget* _parent) {
	const RadialGradientPainter2D* actualPainter = dynamic_cast<const RadialGradientPainter2D*>(_painter);

	m_cLayW = new QWidget(_parent);
	m_cLay = new QVBoxLayout(m_cLayW);

	QGridLayout* positionLay = new QGridLayout;

	Label* centerXLabel = new Label("Center X:", m_cLayW);
	m_centerX = new DoubleSpinBox(m_cLayW);
	m_centerX->setRange(std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max());
	m_centerX->setDecimals(2);
	m_centerX->setSuffix("%");
	m_centerX->setToolTip("The center X position for the gradient. The painted area is between 0.0 and 1.0. The value 2.0 is equal to double the radius.");
	if (actualPainter) m_centerX->setValue(actualPainter->getCenterPoint().x() * 100.);
	positionLay->addWidget(centerXLabel, 0, 0);
	positionLay->addWidget(m_centerX, 0, 1);

	Label* centerYLabel = new Label("Center Y:", m_cLayW);
	m_centerY = new DoubleSpinBox(m_cLayW);
	m_centerY->setRange(std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max());
	m_centerY->setDecimals(2);
	m_centerY->setSuffix("%");
	m_centerY->setToolTip("The center Y position for the gradient. The painted area is between 0.0 and 1.0. The value 2.0 is equal to double the radius.");
	if (actualPainter) m_centerY->setValue(actualPainter->getCenterPoint().y() * 100.);
	positionLay->addWidget(centerYLabel, 1, 0);
	positionLay->addWidget(m_centerY, 1, 1);

	Label* centerRadiusLabel = new Label("Center Radius:", m_cLayW);
	m_centerRadius = new DoubleSpinBox(m_cLayW);
	m_centerRadius->setRange(0., std::numeric_limits<double>::max());
	m_centerRadius->setDecimals(2);
	m_centerRadius->setValue(100.);
	m_centerRadius->setSuffix("%");
	m_centerRadius->setToolTip("The center radius for the gradient. The painted area is between 0.0 and 1.0. The value 2.0 is equal to double the radius.");
	if (actualPainter) m_centerRadius->setValue(actualPainter->getCenterRadius() * 100.);
	positionLay->addWidget(centerRadiusLabel, 2, 0);
	positionLay->addWidget(m_centerRadius, 2, 1);

	Label* focalEnabledLabel = new Label("Focal enabled:", m_cLayW);
	m_useFocal = new CheckBox(m_cLayW);
	if (actualPainter) m_useFocal->setChecked(actualPainter->isFocalPointSet());
	positionLay->addWidget(focalEnabledLabel, 3, 0);
	positionLay->addWidget(m_useFocal, 3, 1);

	Label* focalXLabel = new Label("Focal X:", m_cLayW);
	m_focalX = new DoubleSpinBox(m_cLayW);
	m_focalX->setRange(std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max());
	m_focalX->setDecimals(2);
	m_focalX->setSuffix("%");
	m_focalX->setToolTip("The fianl stop X position for the gradient. The painted area is between 0.0 and 1.0. The value 2.0 is equal to double the radius.");
	if (actualPainter) m_focalX->setValue(actualPainter->getFocalPoint().x() * 100.);
	positionLay->addWidget(focalXLabel, 4, 0);
	positionLay->addWidget(m_focalX, 4, 1);

	Label* focalYLabel = new Label("Focal Y:", m_cLayW);
	m_focalY = new DoubleSpinBox(m_cLayW);
	m_focalY->setRange(std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max());
	m_focalY->setDecimals(2);
	m_focalY->setSuffix("%");
	m_focalY->setToolTip("The fianl stop Y position for the gradient. The painted area is between 0.0 and 1.0. The value 2.0 is equal to double the radius.");
	if (actualPainter) m_focalY->setValue(actualPainter->getFocalPoint().x() * 100.);
	positionLay->addWidget(focalYLabel, 5, 0);
	positionLay->addWidget(m_focalY, 5, 1);

	Label* focalRadiusLabel = new Label("Focal Radius:", m_cLayW);
	m_focalRadius = new DoubleSpinBox(m_cLayW);
	m_focalRadius->setRange(0., std::numeric_limits<double>::max());
	m_focalRadius->setDecimals(2);
	m_focalRadius->setSuffix("%");
	m_focalRadius->setToolTip("The focal radius for the gradient. The painted area is between 0.0 and 1.0. The value 2.0 is equal to double the radius.");
	if (actualPainter) m_focalRadius->setValue(actualPainter->getFocalRadius() * 100.);
	positionLay->addWidget(centerRadiusLabel, 6, 0);
	positionLay->addWidget(m_focalRadius, 6, 1);

	m_cLay->addLayout(positionLay);
	m_gradientBase = new Painter2DEditDialogGradientBase(m_cLay, actualPainter, m_cLayW);

	this->slotFocalEnabledChanged();

	this->connect(m_centerX, &DoubleSpinBox::valueChanged, this, &Painter2DEditDialogEntry::slotValueChanged);
	this->connect(m_centerY, &DoubleSpinBox::valueChanged, this, &Painter2DEditDialogEntry::slotValueChanged);
	this->connect(m_centerRadius, &DoubleSpinBox::valueChanged, this, &Painter2DEditDialogEntry::slotValueChanged);
	this->connect(m_useFocal, &CheckBox::stateChanged, this, &Painter2DEditDialogRadialGradientEntry::slotFocalEnabledChanged);
	this->connect(m_focalX, &DoubleSpinBox::valueChanged, this, &Painter2DEditDialogEntry::slotValueChanged);
	this->connect(m_focalY, &DoubleSpinBox::valueChanged, this, &Painter2DEditDialogEntry::slotValueChanged);
	this->connect(m_focalRadius, &DoubleSpinBox::valueChanged, this, &Painter2DEditDialogEntry::slotValueChanged);
	this->connect(m_gradientBase, &Painter2DEditDialogGradientBase::valuesChanged, this, &Painter2DEditDialogEntry::slotValueChanged);
}

ot::Painter2DEditDialogRadialGradientEntry::~Painter2DEditDialogRadialGradientEntry() {
	delete m_cLayW;
}

ot::Painter2D* ot::Painter2DEditDialogRadialGradientEntry::createPainter() const {
	ot::RadialGradientPainter2D* newPainter = new ot::RadialGradientPainter2D;
	newPainter->setCenterPoint(ot::Point2DD(m_centerX->value() / 100., m_centerY->value() / 100.));
	newPainter->setCenterRadius(m_centerRadius->value() / 100.);
	if (m_useFocal->isChecked()) {
		newPainter->setFocalPoint(ot::Point2DD(m_centerX->value() / 100., m_centerY->value() / 100.));
		newPainter->setFocalRadius(m_centerRadius->value() / 100.);
	}
	newPainter->setStops(m_gradientBase->stops());
	newPainter->setSpread(m_gradientBase->gradientSpread());

	return newPainter;
}

void ot::Painter2DEditDialogRadialGradientEntry::slotFocalEnabledChanged() {
	m_focalX->setEnabled(m_useFocal->isChecked());
	m_focalY->setEnabled(m_useFocal->isChecked());
	m_focalRadius->setEnabled(m_useFocal->isChecked());
}
