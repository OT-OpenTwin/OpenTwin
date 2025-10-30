// @otlicense

// OpenTwin header
#include "OTGui/RadialGradientPainter2D.h"
#include "OTWidgets/CheckBox.h"
#include "OTWidgets/DoubleSpinBox.h"
#include "OTWidgets/Painter2DEditDialogGradientBase.h"
#include "OTWidgets/Painter2DEditDialogRadialGradientEntry.h"

// Qt header
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qlayout.h>

ot::Painter2DEditDialogRadialGradientEntry::Painter2DEditDialogRadialGradientEntry(const Painter2D* _painter) {
	const RadialGradientPainter2D* actualPainter = dynamic_cast<const RadialGradientPainter2D*>(_painter);

	m_cLayW = new QWidget;
	m_cLay = new QVBoxLayout(m_cLayW);

	QGridLayout* positionLay = new QGridLayout;

	QLabel* centerXLabel = new QLabel("Center X:");
	m_centerX = new DoubleSpinBox;
	m_centerX->setRange(std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max());
	m_centerX->setDecimals(2);
	m_centerX->setSuffix("%");
	m_centerX->setToolTip("The center X position for the gradient. The painted area is between 0.0 and 1.0. The value 2.0 is equal to double the radius.");
	if (actualPainter) m_centerX->setValue(actualPainter->getCenterPoint().x() * 100.);
	positionLay->addWidget(centerXLabel, 0, 0);
	positionLay->addWidget(m_centerX, 0, 1);

	QLabel* centerYLabel = new QLabel("Center Y:");
	m_centerY = new DoubleSpinBox;
	m_centerY->setRange(std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max());
	m_centerY->setDecimals(2);
	m_centerY->setSuffix("%");
	m_centerY->setToolTip("The center Y position for the gradient. The painted area is between 0.0 and 1.0. The value 2.0 is equal to double the radius.");
	if (actualPainter) m_centerY->setValue(actualPainter->getCenterPoint().y() * 100.);
	positionLay->addWidget(centerYLabel, 1, 0);
	positionLay->addWidget(m_centerY, 1, 1);

	QLabel* centerRadiusLabel = new QLabel("Center Radius:");
	m_centerRadius = new DoubleSpinBox;
	m_centerRadius->setRange(0., std::numeric_limits<double>::max());
	m_centerRadius->setDecimals(2);
	m_centerRadius->setValue(100.);
	m_centerRadius->setSuffix("%");
	m_centerRadius->setToolTip("The center radius for the gradient. The painted area is between 0.0 and 1.0. The value 2.0 is equal to double the radius.");
	if (actualPainter) m_centerRadius->setValue(actualPainter->getCenterRadius() * 100.);
	positionLay->addWidget(centerRadiusLabel, 2, 0);
	positionLay->addWidget(m_centerRadius, 2, 1);

	QLabel* focalEnabledLabel = new QLabel("Focal enabled:");
	m_useFocal = new CheckBox;
	if (actualPainter) m_useFocal->setChecked(actualPainter->isFocalPointSet());
	positionLay->addWidget(focalEnabledLabel, 3, 0);
	positionLay->addWidget(m_useFocal, 3, 1);

	QLabel* focalXLabel = new QLabel("Focal X:");
	m_focalX = new DoubleSpinBox;
	m_focalX->setRange(std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max());
	m_focalX->setDecimals(2);
	m_focalX->setSuffix("%");
	m_focalX->setToolTip("The fianl stop X position for the gradient. The painted area is between 0.0 and 1.0. The value 2.0 is equal to double the radius.");
	if (actualPainter) m_focalX->setValue(actualPainter->getFocalPoint().x() * 100.);
	positionLay->addWidget(focalXLabel, 4, 0);
	positionLay->addWidget(m_focalX, 4, 1);

	QLabel* focalYLabel = new QLabel("Focal Y:");
	m_focalY = new DoubleSpinBox;
	m_focalY->setRange(std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max());
	m_focalY->setDecimals(2);
	m_focalY->setSuffix("%");
	m_focalY->setToolTip("The fianl stop Y position for the gradient. The painted area is between 0.0 and 1.0. The value 2.0 is equal to double the radius.");
	if (actualPainter) m_focalY->setValue(actualPainter->getFocalPoint().x() * 100.);
	positionLay->addWidget(focalYLabel, 5, 0);
	positionLay->addWidget(m_focalY, 5, 1);

	QLabel* focalRadiusLabel = new QLabel("Focal Radius:");
	m_focalRadius = new DoubleSpinBox;
	m_focalRadius->setRange(0., std::numeric_limits<double>::max());
	m_focalRadius->setDecimals(2);
	m_focalRadius->setSuffix("%");
	m_focalRadius->setToolTip("The focal radius for the gradient. The painted area is between 0.0 and 1.0. The value 2.0 is equal to double the radius.");
	if (actualPainter) m_focalRadius->setValue(actualPainter->getFocalRadius() * 100.);
	positionLay->addWidget(centerRadiusLabel, 6, 0);
	positionLay->addWidget(m_focalRadius, 6, 1);

	m_cLay->addLayout(positionLay);
	m_gradientBase = new Painter2DEditDialogGradientBase(m_cLay, actualPainter);

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
