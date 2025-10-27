//! @file Painter2DEditDialogGradientBase.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTGui/GradientPainter2D.h"
#include "OTWidgets/SpinBox.h"
#include "OTWidgets/ComboBox.h"
#include "OTWidgets/DoubleSpinBox.h"
#include "OTWidgets/ColorPickButton.h"
#include "OTWidgets/Painter2DEditDialogGradientBase.h"

// Qt header
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qgroupbox.h>

ot::Painter2DEditDialogGradientBase::Painter2DEditDialogGradientBase(QVBoxLayout* _layout, const GradientPainter2D* _painter) :
	m_layout(_layout) {
	std::vector<GradientPainterStop2D> tmp;
	if (_painter) {
		tmp = _painter->getStops();
	}
	if (tmp.empty()) {
		tmp.push_back(GradientPainterStop2D());
	}

	QStringList itms;
	itms.append(QString::fromStdString(toString(GradientSpread::Pad)));
	itms.append(QString::fromStdString(toString(GradientSpread::Repeat)));
	itms.append(QString::fromStdString(toString(GradientSpread::Reflect)));
	m_spreadBox = new ComboBox;
	m_spreadBox->setEditable(false);
	m_spreadBox->addItems(itms);

	QLabel* spreadLabel = new QLabel("Spread:");
	QHBoxLayout* spreadLay = new QHBoxLayout;
	spreadLay->addWidget(spreadLabel);
	spreadLay->addWidget(m_spreadBox, 1);

	QHBoxLayout* stopsLay = new QHBoxLayout;
	QLabel* stopsLabel = new QLabel("Stops:");

	if (_painter) m_spreadBox->setCurrentText(QString::fromStdString(toString(_painter->getSpread())));
	else m_spreadBox->setCurrentText(QString::fromStdString(toString(GradientSpread::Pad)));
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

ot::Painter2DEditDialogGradientBase::~Painter2DEditDialogGradientBase() {
	for (const StopEntry& e : m_stops) {
		this->disconnect(e.pos, &DoubleSpinBox::valueChanged, this, &Painter2DEditDialogGradientBase::slotValueChanged);
		this->disconnect(e.color, &ColorPickButton::colorChanged, this, &Painter2DEditDialogGradientBase::slotValueChanged);
	}
}

void ot::Painter2DEditDialogGradientBase::addStop(const GradientPainterStop2D& _stop) {
	std::vector<GradientPainterStop2D> newStops = this->stops();
	newStops.push_back(_stop);
	this->rebuild(newStops);
}

void ot::Painter2DEditDialogGradientBase::addStops(const std::vector<GradientPainterStop2D>& _stops) {
	std::vector<GradientPainterStop2D> newStops = this->stops();
	newStops.resize(newStops.size() + _stops.size());
	for (const GradientPainterStop2D& s : _stops) newStops.push_back(s);
	this->rebuild(newStops);
}

std::vector<ot::GradientPainterStop2D> ot::Painter2DEditDialogGradientBase::stops() const {
	std::vector<ot::GradientPainterStop2D> ret;
	for (const StopEntry& e : m_stops) {
		ret.push_back(this->createStopFromEntry(e));
	}
	return ret;
}

ot::GradientSpread ot::Painter2DEditDialogGradientBase::gradientSpread() {
	if (m_spreadBox->currentText().toStdString() == ot::toString(GradientSpread::Pad)) return GradientSpread::Pad;
	else if (m_spreadBox->currentText().toStdString() == ot::toString(GradientSpread::Repeat)) return GradientSpread::Repeat;
	else if (m_spreadBox->currentText().toStdString() == ot::toString(GradientSpread::Reflect)) return GradientSpread::Reflect;
	else {
		OT_LOG_EA("Unknown pad spread");
		return GradientSpread::Pad;
	}
}

void ot::Painter2DEditDialogGradientBase::slotCountChanged(int _newCount) {
	std::vector<ot::GradientPainterStop2D> newData = this->stops();
	newData.resize(_newCount, GradientPainterStop2D());
	this->rebuild(newData);
	Q_EMIT valuesChanged();
}

void ot::Painter2DEditDialogGradientBase::slotValueChanged() {
	Q_EMIT valuesChanged();
}

void ot::Painter2DEditDialogGradientBase::rebuild(const std::vector<GradientPainterStop2D>& _stops) {
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

ot::Painter2DEditDialogGradientBase::StopEntry ot::Painter2DEditDialogGradientBase::createStopEntry(size_t stopIx, const GradientPainterStop2D& _stop) const {
	StopEntry newEntry;

	newEntry.box = new QGroupBox("Stop " + QString::number(stopIx + 1));
	QGridLayout* lay = new QGridLayout(newEntry.box);
	QLabel* positionLabel = new QLabel("Position:");
	QLabel* colorLabel = new QLabel("Color:");

	newEntry.pos = new DoubleSpinBox;
	newEntry.pos->setRange(0., 100.);
	newEntry.pos->setDecimals(2);
	newEntry.pos->setValue((_stop.getPos() >= 0. && _stop.getPos() <= 1.) ? (_stop.getPos() * 100.) : 0.);
	newEntry.pos->setSuffix("%");

	newEntry.color = new ColorPickButton(_stop.getColor());

	lay->addWidget(positionLabel, 0, 0);
	lay->addWidget(newEntry.pos, 0, 1);
	lay->addWidget(colorLabel, 1, 0);
	lay->addWidget(newEntry.color, 1, 1);
	lay->setColumnStretch(1, 1);

	return newEntry;
}

ot::GradientPainterStop2D ot::Painter2DEditDialogGradientBase::createStopFromEntry(const StopEntry& _entry) const {
	return ot::GradientPainterStop2D(_entry.pos->value() / 100., _entry.color->otColor());
}
