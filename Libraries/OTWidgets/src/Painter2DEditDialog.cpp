//! @file Painter2DEditDialog.h
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/Painter2D.h"
#include "OTGui/FillPainter2D.h"
#include "OTGui/LinearGradientPainter2D.h"
#include "OTGui/RadialGradientPainter2D.h"
#include "OTWidgets/SpinBox.h"
#include "OTWidgets/CheckBox.h"
#include "OTWidgets/ComboBox.h"
#include "OTWidgets/PushButton.h"
#include "OTWidgets/DoubleSpinBox.h"
#include "OTWidgets/ColorStyleTypes.h"
#include "OTWidgets/ColorPickButton.h"
#include "OTWidgets/GlobalColorStyle.h"
#include "OTWidgets/Painter2DPreview.h"
#include "OTWidgets/Painter2DFactory.h"
#include "OTWidgets/Painter2DEditDialog.h"

// Qt header
#include <QtGui/qevent.h>
#include <QtGui/qpainter.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qgroupbox.h>
#include <QtWidgets/qscrollarea.h>
#include <QtWidgets/qsizepolicy.h>

#define P2DED_Fill "Fill"
#define P2DED_Linear "Linear"
#define P2DED_Radial "Radial"

ot::Painter2DEditDialogEntry::~Painter2DEditDialogEntry() {}

void ot::Painter2DEditDialogEntry::slotValueChanged(void) {
	Q_EMIT valueChanged();
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::Painter2DEditDialogFillEntry::Painter2DEditDialogFillEntry(const Painter2D* _painter)
{
	m_btn = new ColorPickButton;
	if (_painter) {
		const FillPainter2D* actualPainter = dynamic_cast<const FillPainter2D*>(_painter);
		OTAssertNullptr(actualPainter);
		m_btn->setColor(actualPainter->color());
	}
	this->connect(m_btn, &ColorPickButton::colorChanged, this, &Painter2DEditDialogEntry::slotValueChanged);
}

ot::Painter2DEditDialogFillEntry::~Painter2DEditDialogFillEntry() {
	delete m_btn;
}

QWidget* ot::Painter2DEditDialogFillEntry::getRootWidget(void) const {
	return m_btn;
}

ot::Painter2D* ot::Painter2DEditDialogFillEntry::createPainter(void) const {
	return new FillPainter2D(m_btn->otColor());
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::Painter2DEditDialogLinearGradientEntry::Painter2DEditDialogLinearGradientEntry(const Painter2D* _painter) 
{
	const LinearGradientPainter2D* actualPainter = dynamic_cast<const LinearGradientPainter2D*>(_painter);

	m_cLayW = new QWidget;
	m_cLay = new QVBoxLayout(m_cLayW);

	QGridLayout* positionLay = new QGridLayout;

	QLabel* startXLabel = new QLabel("Start X:");
	m_startX = new DoubleSpinBox;
	m_startX->setRange(DBL_MIN, DBL_MAX);
	m_startX->setDecimals(2);
	m_startX->setSuffix("%");
	m_startX->setToolTip("The start X position for the gradient. The painted area is between 0.0 and 1.0. The value 2.0 is equal to double the width and -1.0 the width in negative direction.");
	if (actualPainter) m_startX->setValue(actualPainter->start().x() * 100.);
	positionLay->addWidget(startXLabel, 0, 0);
	positionLay->addWidget(m_startX, 0, 1);

	QLabel* startYLabel = new QLabel("Start Y:");
	m_startY = new DoubleSpinBox;
	m_startY->setRange(DBL_MIN, DBL_MAX);
	m_startY->setDecimals(2);
	m_startY->setSuffix("%");
	m_startY->setToolTip("The start Y position for the gradient. The painted area is between 0.0 and 1.0. The value 2.0 is equal to double the height and -1.0 the height in negative direction.");
	if (actualPainter) m_startY->setValue(actualPainter->start().y() * 100.);
	positionLay->addWidget(startYLabel, 1, 0);
	positionLay->addWidget(m_startY, 1, 1);

	QLabel* endXLabel = new QLabel("End X:");
	m_finalX = new DoubleSpinBox;
	m_finalX->setRange(DBL_MIN, DBL_MAX);
	m_finalX->setDecimals(2);
	m_finalX->setValue(100.);
	m_finalX->setSuffix("%");
	m_finalX->setToolTip("The fianl stop X position for the gradient. The painted area is between 0.0 and 1.0. The value 2.0 is equal to double the width and -1.0 the width in negative direction.");
	if (actualPainter) m_finalX->setValue(actualPainter->finalStop().x() * 100.);
	positionLay->addWidget(endXLabel, 2, 0);
	positionLay->addWidget(m_finalX, 2, 1);

	QLabel* endYLabel = new QLabel("End Y:");
	m_finalY = new DoubleSpinBox;
	m_finalY->setRange(DBL_MIN, DBL_MAX);
	m_finalY->setDecimals(2);
	m_finalY->setValue(100.);
	m_finalX->setSuffix("%");
	m_finalY->setToolTip("The fianl stop Y position for the gradient. The painted area is between 0.0 and 1.0. The value 2.0 is equal to double the height and -1.0 the height in negative direction.");
	if (actualPainter) m_finalY->setValue(actualPainter->finalStop().y() * 100.);
	positionLay->addWidget(endYLabel, 3, 0);
	positionLay->addWidget(m_finalY, 3, 1);

	m_cLay->addLayout(positionLay);
	m_gradientBase = new intern::Painter2DEditDialogGradientBase(m_cLay, actualPainter);

	this->connect(m_startX, &DoubleSpinBox::valueChanged, this, &Painter2DEditDialogEntry::slotValueChanged);
	this->connect(m_startY, &DoubleSpinBox::valueChanged, this, &Painter2DEditDialogEntry::slotValueChanged);
	this->connect(m_finalX, &DoubleSpinBox::valueChanged, this, &Painter2DEditDialogEntry::slotValueChanged);
	this->connect(m_finalY, &DoubleSpinBox::valueChanged, this, &Painter2DEditDialogEntry::slotValueChanged);
	this->connect(m_gradientBase, &intern::Painter2DEditDialogGradientBase::valuesChanged, this, &Painter2DEditDialogEntry::slotValueChanged);
}

ot::Painter2DEditDialogLinearGradientEntry::~Painter2DEditDialogLinearGradientEntry() {
	delete m_cLayW;
}

ot::Painter2D* ot::Painter2DEditDialogLinearGradientEntry::createPainter(void) const {
	ot::LinearGradientPainter2D* newPainter = new ot::LinearGradientPainter2D;
	newPainter->setStart(ot::Point2DD(m_startX->value() / 100., m_startY->value() / 100.));
	newPainter->setFinalStop(ot::Point2DD(m_finalX->value() / 100., m_finalY->value() / 100.));
	newPainter->setStops(m_gradientBase->stops());
	newPainter->setSpread(m_gradientBase->gradientSpread());
	return newPainter;
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::Painter2DEditDialogRadialGradientEntry::Painter2DEditDialogRadialGradientEntry(const Painter2D* _painter)
{
	const RadialGradientPainter2D* actualPainter = dynamic_cast<const RadialGradientPainter2D*>(_painter);

	m_cLayW = new QWidget;
	m_cLay = new QVBoxLayout(m_cLayW);

	QGridLayout* positionLay = new QGridLayout;

	QLabel* centerXLabel = new QLabel("Center X:");
	m_centerX = new DoubleSpinBox;
	m_centerX->setRange(DBL_MIN, DBL_MAX);
	m_centerX->setDecimals(2);
	m_centerX->setSuffix("%");
	m_centerX->setToolTip("The center X position for the gradient. The painted area is between 0.0 and 1.0. The value 2.0 is equal to double the radius.");
	if (actualPainter) m_centerX->setValue(actualPainter->centerPoint().x() * 100.);
	positionLay->addWidget(centerXLabel, 0, 0);
	positionLay->addWidget(m_centerX, 0, 1);

	QLabel* centerYLabel = new QLabel("Center Y:");
	m_centerY = new DoubleSpinBox;
	m_centerY->setRange(DBL_MIN, DBL_MAX);
	m_centerY->setDecimals(2);
	m_centerY->setSuffix("%");
	m_centerY->setToolTip("The center Y position for the gradient. The painted area is between 0.0 and 1.0. The value 2.0 is equal to double the radius.");
	if (actualPainter) m_centerY->setValue(actualPainter->centerPoint().y() * 100.);
	positionLay->addWidget(centerYLabel, 1, 0);
	positionLay->addWidget(m_centerY, 1, 1);

	QLabel* centerRadiusLabel = new QLabel("Center Radius:");
	m_centerRadius = new DoubleSpinBox;
	m_centerRadius->setRange(0., DBL_MAX);
	m_centerRadius->setDecimals(2);
	m_centerRadius->setValue(100.);
	m_centerRadius->setSuffix("%");
	m_centerRadius->setToolTip("The center radius for the gradient. The painted area is between 0.0 and 1.0. The value 2.0 is equal to double the radius.");
	if (actualPainter) m_centerRadius->setValue(actualPainter->centerRadius() * 100.);
	positionLay->addWidget(centerRadiusLabel, 2, 0);
	positionLay->addWidget(m_centerRadius, 2, 1);

	QLabel* focalEnabledLabel = new QLabel("Focal enabled:");
	m_useFocal = new CheckBox;
	if (actualPainter) m_useFocal->setChecked(actualPainter->isFocalPointSet());
	positionLay->addWidget(focalEnabledLabel, 3, 0);
	positionLay->addWidget(m_useFocal, 3, 1);

	QLabel* focalXLabel = new QLabel("Focal X:");
	m_focalX = new DoubleSpinBox;
	m_focalX->setRange(DBL_MIN, DBL_MAX);
	m_focalX->setDecimals(2);
	m_focalX->setSuffix("%");
	m_focalX->setToolTip("The fianl stop X position for the gradient. The painted area is between 0.0 and 1.0. The value 2.0 is equal to double the radius.");
	if (actualPainter) m_focalX->setValue(actualPainter->focalPoint().x() * 100.);
	positionLay->addWidget(focalXLabel, 4, 0);
	positionLay->addWidget(m_focalX, 4, 1);

	QLabel* focalYLabel = new QLabel("Focal Y:");
	m_focalY = new DoubleSpinBox;
	m_focalY->setRange(DBL_MIN, DBL_MAX);
	m_focalY->setDecimals(2);
	m_focalY->setSuffix("%");
	m_focalY->setToolTip("The fianl stop Y position for the gradient. The painted area is between 0.0 and 1.0. The value 2.0 is equal to double the radius.");
	if (actualPainter) m_focalY->setValue(actualPainter->focalPoint().x() * 100.);
	positionLay->addWidget(focalYLabel, 5, 0);
	positionLay->addWidget(m_focalY, 5, 1);

	QLabel* focalRadiusLabel = new QLabel("Focal Radius:");
	m_focalRadius = new DoubleSpinBox;
	m_focalRadius->setRange(0., DBL_MAX);
	m_focalRadius->setDecimals(2);
	m_focalRadius->setSuffix("%");
	m_focalRadius->setToolTip("The focal radius for the gradient. The painted area is between 0.0 and 1.0. The value 2.0 is equal to double the radius.");
	if (actualPainter) m_focalRadius->setValue(actualPainter->focalRadius() * 100.);
	positionLay->addWidget(centerRadiusLabel, 6, 0);
	positionLay->addWidget(m_focalRadius, 6, 1);

	m_cLay->addLayout(positionLay);
	m_gradientBase = new intern::Painter2DEditDialogGradientBase(m_cLay, actualPainter);

	this->slotFocalEnabledChanged();

	this->connect(m_centerX, &DoubleSpinBox::valueChanged, this, &Painter2DEditDialogEntry::slotValueChanged);
	this->connect(m_centerY, &DoubleSpinBox::valueChanged, this, &Painter2DEditDialogEntry::slotValueChanged);
	this->connect(m_centerRadius, &DoubleSpinBox::valueChanged, this, &Painter2DEditDialogEntry::slotValueChanged);
	this->connect(m_useFocal, &CheckBox::stateChanged, this, &Painter2DEditDialogRadialGradientEntry::slotFocalEnabledChanged);
	this->connect(m_focalX, &DoubleSpinBox::valueChanged, this, &Painter2DEditDialogEntry::slotValueChanged);
	this->connect(m_focalY, &DoubleSpinBox::valueChanged, this, &Painter2DEditDialogEntry::slotValueChanged);
	this->connect(m_focalRadius, &DoubleSpinBox::valueChanged, this, &Painter2DEditDialogEntry::slotValueChanged);
	this->connect(m_gradientBase, &intern::Painter2DEditDialogGradientBase::valuesChanged, this, &Painter2DEditDialogEntry::slotValueChanged);
}

ot::Painter2DEditDialogRadialGradientEntry::~Painter2DEditDialogRadialGradientEntry() {
	delete m_cLayW;
}

ot::Painter2D* ot::Painter2DEditDialogRadialGradientEntry::createPainter(void) const {
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

void ot::Painter2DEditDialogRadialGradientEntry::slotFocalEnabledChanged(void) {
	m_focalX->setEnabled(m_useFocal->isChecked());
	m_focalY->setEnabled(m_useFocal->isChecked());
	m_focalRadius->setEnabled(m_useFocal->isChecked());
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::Painter2DEditDialog::Painter2DEditDialog(const Painter2D* _painter) 
	: m_currentEntry(nullptr), m_changed(false)
{
	// Initialize data
	if (_painter) m_painter = _painter->createCopy();
	else m_painter = new FillPainter2D;

	this->ini();
}

ot::Painter2DEditDialog::~Painter2DEditDialog() {
	delete m_painter;
}

ot::Painter2D* ot::Painter2DEditDialog::createPainter(void) const {
	if (m_currentEntry) return m_currentEntry->createPainter();
	else return nullptr;
}

void ot::Painter2DEditDialog::slotTypeChanged() {
	ot::Painter2D* newPainter = new FillPainter2D;
	if (m_typeSelectionBox->currentText() == P2DED_Fill) {}
	else if (m_typeSelectionBox->currentText() == P2DED_Linear) newPainter = new LinearGradientPainter2D;
	else if (m_typeSelectionBox->currentText() == P2DED_Radial) newPainter = new RadialGradientPainter2D;
	else {
		OT_LOG_EA("Unknown entry type");
	}
	if (!newPainter) return;
	if (m_painter) delete m_painter;
	m_painter = newPainter;
	this->applyPainter(m_painter);
	this->slotUpdate();
}

void ot::Painter2DEditDialog::slotUpdate(void) {
	if (m_currentEntry) {
		Painter2D* newPainter = this->createPainter();
		if (newPainter) m_painter = newPainter;
		m_preview->setFromPainter((const Painter2D*)m_painter);
		m_preview->repaint();
		m_changed = true;
		Q_EMIT painterChanged();
	}
}

void ot::Painter2DEditDialog::slotConfirm(void) {
	if (m_changed) {
		this->close(Dialog::Ok);
	}
	else {
		this->close(Dialog::Cancel);
	}
}

void ot::Painter2DEditDialog::ini(void) {
	// Create layouts
	QVBoxLayout* cLay = new QVBoxLayout;
	QHBoxLayout* hLay = new QHBoxLayout;
	QScrollArea* sArea = new QScrollArea;
	QWidget* vLayW = new QWidget;
	m_vLayout = new QVBoxLayout(vLayW);
	sArea->setMinimumWidth(300);
	sArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	sArea->setWidgetResizable(true);
	sArea->setWidget(vLayW);
	QHBoxLayout* comboLay = new QHBoxLayout;
	hLay->addWidget(sArea);

	// Create default widgets
	QLabel* typeLabel = new QLabel("Type:");
	m_typeSelectionBox = new ComboBox;
	m_typeSelectionBox->setEditable(false);
	m_preview = new Painter2DPreview;
	m_preview->setMinimumSize(48, 48);
	m_preview->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	m_preview->setMaintainAspectRatio(true);
	m_stretchItem = new QWidget;
	hLay->addWidget(m_preview, 1);

	m_confirm = new PushButton("Confirm");
	m_cancel = new PushButton("Cancel");
	cLay->addLayout(hLay, 1);
	cLay->addWidget(m_confirm);
	cLay->addWidget(m_cancel);

	// Setup controls
	m_typeSelectionBox->addItems(QStringList({ P2DED_Fill, P2DED_Linear, P2DED_Radial }));
	m_typeSelectionBox->setCurrentIndex(0);

	// Setup layouts
	comboLay->addWidget(typeLabel);
	comboLay->addWidget(m_typeSelectionBox, 1);
	m_vLayout->addLayout(comboLay, 0);

	this->setMinimumSize(600, 600);

	this->applyPainter(m_painter);
	this->slotUpdate();
	m_changed = false;
	this->connect(m_confirm, &PushButton::clicked, this, &Painter2DEditDialog::slotConfirm);
	this->connect(m_cancel, &PushButton::clicked, this, &Dialog::closeCancel);
	this->connect(m_typeSelectionBox, &ComboBox::currentTextChanged, this, &Painter2DEditDialog::slotTypeChanged);

	this->setLayout(cLay);
}

void ot::Painter2DEditDialog::applyPainter(const Painter2D* _painter) {
	if (m_currentEntry) {
		this->disconnect(m_currentEntry, &Painter2DEditDialogEntry::valueChanged, this, &Painter2DEditDialog::slotUpdate);
		m_currentEntry->getRootWidget()->hide();
		m_stretchItem->hide();
		m_vLayout->removeWidget(m_stretchItem);
		m_vLayout->removeWidget(m_currentEntry->getRootWidget());
		delete m_currentEntry;
		m_currentEntry = nullptr;
	}
	if (_painter->simpleFactoryObjectKey() == OT_SimpleFactoryJsonKeyValue_FillPainter2DCfg) m_currentEntry = new Painter2DEditDialogFillEntry(_painter);
	else if (_painter->simpleFactoryObjectKey() == OT_SimpleFactoryJsonKeyValue_LinearGradientPainter2DCfg) m_currentEntry = new Painter2DEditDialogLinearGradientEntry(_painter);
	else if (_painter->simpleFactoryObjectKey() == OT_SimpleFactoryJsonKeyValue_RadialGradientPainter2DCfg) m_currentEntry = new Painter2DEditDialogRadialGradientEntry(_painter);
	else {
		OT_LOG_E("Unknown painter type");
	}
	if (m_currentEntry) {
		m_vLayout->addWidget(m_currentEntry->getRootWidget());
		m_vLayout->addWidget(m_stretchItem, 1);
		m_stretchItem->setHidden(false);
		this->connect(m_currentEntry, &Painter2DEditDialogEntry::valueChanged, this, &Painter2DEditDialog::slotUpdate);
		m_vLayout->update();
		this->update();
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::intern::Painter2DEditDialogGradientBase::Painter2DEditDialogGradientBase(QVBoxLayout* _layout, const GradientPainter2D* _painter)
	: m_layout(_layout)
{
	std::vector<GradientPainterStop2D> tmp;
	if (_painter) {
		tmp = _painter->stops();
	}
	if (tmp.empty()) {
		tmp.push_back(GradientPainterStop2D());
	}

	QStringList itms;
	itms.append(QString::fromStdString(toString(PadSpread)));
	itms.append(QString::fromStdString(toString(RepeatSpread)));
	itms.append(QString::fromStdString(toString(ReflectSpread)));
	m_spreadBox = new ComboBox;
	m_spreadBox->setEditable(false);
	m_spreadBox->addItems(itms);
	
	QLabel* spreadLabel = new QLabel("Spread:");
	QHBoxLayout* spreadLay = new QHBoxLayout;
	spreadLay->addWidget(spreadLabel);
	spreadLay->addWidget(m_spreadBox, 1);

	QHBoxLayout* stopsLay = new QHBoxLayout;
	QLabel* stopsLabel = new QLabel("Stops:");
	
	if (_painter) m_spreadBox->setCurrentText(QString::fromStdString(toString(_painter->spread())));
	else m_spreadBox->setCurrentText(QString::fromStdString(toString(PadSpread)));
	m_stopsBox = new SpinBox;
	m_stopsBox->setValue(tmp.size());
	m_stopsBox->setRange(1, 99);
	stopsLay->addWidget(stopsLabel, 0);
	stopsLay->addWidget(m_stopsBox, 1);

	m_layout->addLayout(spreadLay);
	m_layout->addLayout(stopsLay);

	this->rebuild(tmp);
	this->connect(m_spreadBox, &ComboBox::currentTextChanged, this, &Painter2DEditDialogGradientBase::slotValueChanged);
	this->connect(m_stopsBox, &SpinBox::valueChanged, this, &Painter2DEditDialogGradientBase::slotCountChanged);
}

ot::intern::Painter2DEditDialogGradientBase::~Painter2DEditDialogGradientBase() {
	for (const StopEntry& e : m_stops) {
		this->disconnect(e.pos, &DoubleSpinBox::valueChanged, this, &Painter2DEditDialogGradientBase::slotValueChanged);
		this->disconnect(e.color, &ColorPickButton::colorChanged, this, &Painter2DEditDialogGradientBase::slotValueChanged);
	}
}

void ot::intern::Painter2DEditDialogGradientBase::addStop(const GradientPainterStop2D& _stop) {
	std::vector<GradientPainterStop2D> newStops = this->stops();
	newStops.push_back(_stop);
	this->rebuild(newStops);
}

void ot::intern::Painter2DEditDialogGradientBase::addStops(const std::vector<GradientPainterStop2D>& _stops) {
	std::vector<GradientPainterStop2D> newStops = this->stops();
	newStops.resize(newStops.size() + _stops.size());
	for (const GradientPainterStop2D& s : _stops) newStops.push_back(s);
	this->rebuild(newStops);
}

std::vector<ot::GradientPainterStop2D> ot::intern::Painter2DEditDialogGradientBase::stops(void) const {
	std::vector<ot::GradientPainterStop2D> ret;
	for (const StopEntry& e : m_stops) {
		ret.push_back(this->createStopFromEntry(e));
	}
	return ret;
}

ot::GradientSpread ot::intern::Painter2DEditDialogGradientBase::gradientSpread(void) {
	if (m_spreadBox->currentText().toStdString() == ot::toString(PadSpread)) return PadSpread;
	else if (m_spreadBox->currentText().toStdString() == ot::toString(RepeatSpread)) return RepeatSpread;
	else if (m_spreadBox->currentText().toStdString() == ot::toString(ReflectSpread)) return ReflectSpread;
	else {
		OT_LOG_EA("Unknown pad spread");
		return PadSpread;
	}
}

void ot::intern::Painter2DEditDialogGradientBase::slotCountChanged(int _newCount) {
	std::vector<ot::GradientPainterStop2D> newData = this->stops();
	newData.resize(_newCount, GradientPainterStop2D());
	this->rebuild(newData);
	Q_EMIT valuesChanged();
}

void ot::intern::Painter2DEditDialogGradientBase::slotValueChanged(void) {
	Q_EMIT valuesChanged();
}

void ot::intern::Painter2DEditDialogGradientBase::rebuild(const std::vector<GradientPainterStop2D>& _stops) {
	for (const StopEntry& e : m_stops) {
		this->disconnect(e.pos, &DoubleSpinBox::valueChanged, this, &Painter2DEditDialogGradientBase::slotValueChanged);
		this->disconnect(e.color, &ColorPickButton::colorChanged, this, &Painter2DEditDialogGradientBase::slotValueChanged);
		e.box->hide();
		m_layout->removeWidget(e.box);
		delete e.box;
	}

	m_stops.clear();
	for (size_t ix = 0; ix < _stops.size(); ix++) {
		StopEntry newEntry = this->createStopEntry(ix, _stops[ix]);
		m_layout->addWidget(newEntry.box);
		m_stops.push_back(newEntry);

		this->connect(newEntry.pos, &DoubleSpinBox::valueChanged, this, &Painter2DEditDialogGradientBase::slotValueChanged);
		this->connect(newEntry.color, &ColorPickButton::colorChanged, this, &Painter2DEditDialogGradientBase::slotValueChanged);
	}
}

ot::intern::Painter2DEditDialogGradientBase::StopEntry ot::intern::Painter2DEditDialogGradientBase::createStopEntry(size_t stopIx, const GradientPainterStop2D& _stop) const {
	StopEntry newEntry;

	newEntry.box = new QGroupBox("Stop " + QString::number(stopIx + 1));
	QGridLayout* lay = new QGridLayout(newEntry.box);
	QLabel* positionLabel = new QLabel("Position:");
	QLabel* colorLabel = new QLabel("Color:");

	newEntry.pos = new DoubleSpinBox;
	newEntry.pos->setRange(0., 100.);
	newEntry.pos->setDecimals(2);
	newEntry.pos->setValue((_stop.pos() >= 0. && _stop.pos() <= 1.) ? (_stop.pos() *  100.) : 0.);
	newEntry.pos->setSuffix("%");

	newEntry.color = new ColorPickButton(_stop.color());

	lay->addWidget(positionLabel, 0, 0);
	lay->addWidget(newEntry.pos, 0, 1);
	lay->addWidget(colorLabel, 1, 0);
	lay->addWidget(newEntry.color, 1, 1);
	lay->setColumnStretch(1, 1);

	return newEntry;
}

ot::GradientPainterStop2D ot::intern::Painter2DEditDialogGradientBase::createStopFromEntry(const StopEntry& _entry) const {
	return ot::GradientPainterStop2D(_entry.pos->value() / 100., _entry.color->otColor());
}