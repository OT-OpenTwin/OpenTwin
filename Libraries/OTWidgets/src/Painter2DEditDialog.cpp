//! @file Painter2DEditDialog.h
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/Painter2D.h"
#include "OTGui/FillPainter2D.h"
#include "OTGui/ColorStyleTypes.h"
#include "OTGui/StyleRefPainter2D.h"
#include "OTGui/LinearGradientPainter2D.h"
#include "OTGui/RadialGradientPainter2D.h"
#include "OTWidgets/ComboBox.h"
#include "OTWidgets/PushButton.h"
#include "OTWidgets/ColorPickButton.h"
#include "OTWidgets/GlobalColorStyle.h"
#include "OTWidgets/Painter2DPreview.h"
#include "OTWidgets/Painter2DEditDialog.h"
#include "OTWidgets/Painter2DEditDialogFillEntry.h"
#include "OTWidgets/Painter2DEditDialogReferenceEntry.h"
#include "OTWidgets/Painter2DEditDialogLinearGradientEntry.h"
#include "OTWidgets/Painter2DEditDialogRadialGradientEntry.h"

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
#define P2DED_StyleRef "Style Reference"

ot::Painter2DEditDialog::Painter2DEditDialog(const Painter2DDialogFilter& _filter, const Painter2D* _painter) :
	m_currentEntry(nullptr), m_changed(false), m_filter(_filter)
{
	// Initialize data
	if (_painter) m_painter = _painter->createCopy();
	else m_painter = new FillPainter2D;

	this->ini();
}

ot::Painter2DEditDialog::~Painter2DEditDialog() {
	delete m_painter;
}

ot::Painter2D* ot::Painter2DEditDialog::createPainter() const {
	if (m_currentEntry) return m_currentEntry->createPainter();
	else return nullptr;
}

void ot::Painter2DEditDialog::slotTypeChanged() {
	ot::Painter2D* newPainter = nullptr;

	if (m_typeSelectionBox->currentText() == P2DED_Fill) { 
		newPainter = new FillPainter2D;
	}
	else if (m_typeSelectionBox->currentText() == P2DED_Linear) {
		newPainter = new LinearGradientPainter2D;
	}
	else if (m_typeSelectionBox->currentText() == P2DED_Radial) {
		newPainter = new RadialGradientPainter2D;
	}
	else if (m_typeSelectionBox->currentText() == P2DED_StyleRef) {
		newPainter = new StyleRefPainter2D;
	}
	else {
		OT_LOG_E("Unknown entry type: \"" + m_typeSelectionBox->currentText().toStdString());
		return;
	}
	
	if (m_painter) {
		delete m_painter;
	}
	m_painter = newPainter;

	this->applyPainter(m_painter);
	this->slotUpdate();
}

void ot::Painter2DEditDialog::slotUpdate() {
	if (m_currentEntry) {
		Painter2D* newPainter = this->createPainter();
		if (newPainter) m_painter = newPainter;
		m_preview->setFromPainter((const Painter2D*)m_painter);
		m_changed = true;
		Q_EMIT painterChanged();
	}
}

void ot::Painter2DEditDialog::slotConfirm() {
	if (m_changed) {
		this->closeDialog(Dialog::Ok);
	}
	else {
		this->closeDialog(Dialog::Cancel);
	}
}

void ot::Painter2DEditDialog::ini() {
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

	// Initialize allowed painter types
	QStringList painterOpt;
	if (m_filter.getPainterTypes() & Painter2DDialogFilter::Fill || m_painter->getFactoryKey() == OT_FactoryKey_FillPainter2D) {
		painterOpt.append(P2DED_Fill);
	}
	if (m_filter.getPainterTypes() & Painter2DDialogFilter::LinearGradient || m_painter->getFactoryKey() == OT_FactoryKey_LinearGradientPainter2D) {
		painterOpt.append(P2DED_Linear);
	}
	if (m_filter.getPainterTypes() & Painter2DDialogFilter::RadialGradient || m_painter->getFactoryKey() == OT_FactoryKey_RadialGradientPainter2D) {
		painterOpt.append(P2DED_Radial);
	}
	if (m_filter.getPainterTypes() & Painter2DDialogFilter::StyleRef || m_painter->getFactoryKey() == OT_FactoryKey_StyleRefPainter2D) {
		painterOpt.append(P2DED_StyleRef);
	}
	m_typeSelectionBox->addItems(painterOpt);

	// Initialize currently selected painter type
	if (m_painter->getFactoryKey() == OT_FactoryKey_FillPainter2D) {
		m_typeSelectionBox->setCurrentText(P2DED_Fill);
	}
	else if (m_painter->getFactoryKey() == OT_FactoryKey_LinearGradientPainter2D) {
		m_typeSelectionBox->setCurrentText(P2DED_Linear);
	}
	else if (m_painter->getFactoryKey() == OT_FactoryKey_RadialGradientPainter2D) {
		m_typeSelectionBox->setCurrentText(P2DED_Radial);
	}
	else if (m_painter->getFactoryKey() == OT_FactoryKey_StyleRefPainter2D) {
		m_typeSelectionBox->setCurrentText(P2DED_StyleRef);
	}
	else {
		OT_LOG_E("Unknown painter \"" + m_painter->getFactoryKey() + "\"");
		m_typeSelectionBox->setCurrentIndex(0);
	}

	// Setup layouts
	comboLay->addWidget(typeLabel);
	comboLay->addWidget(m_typeSelectionBox, 1);
	m_vLayout->addLayout(comboLay, 0);

	this->setMinimumSize(600, 600);

	// Update controls and painter selection
	this->applyPainter(m_painter);
	this->slotUpdate();
	m_changed = false;

	// Connect signals
	this->connect(m_confirm, &PushButton::clicked, this, &Painter2DEditDialog::slotConfirm);
	this->connect(m_cancel, &PushButton::clicked, this, &Dialog::closeCancel);
	this->connect(m_typeSelectionBox, &ComboBox::currentTextChanged, this, &Painter2DEditDialog::slotTypeChanged);

	// Apply layout
	this->setLayout(cLay);
}

void ot::Painter2DEditDialog::applyPainter(const Painter2D* _painter) {
	OTAssertNullptr(_painter);

	// Remove currently set entry
	if (m_currentEntry) {
		this->disconnect(m_currentEntry, &Painter2DEditDialogEntry::valueChanged, this, &Painter2DEditDialog::slotUpdate);
		m_currentEntry->getRootWidget()->hide();
		m_stretchItem->hide();
		m_vLayout->removeWidget(m_stretchItem);
		m_vLayout->removeWidget(m_currentEntry->getRootWidget());
		delete m_currentEntry;
		m_currentEntry = nullptr;
	}

	// Setup new entry
	if (_painter->getFactoryKey() == OT_FactoryKey_FillPainter2D) {
		m_currentEntry = new Painter2DEditDialogFillEntry(_painter);
	}
	else if (_painter->getFactoryKey() == OT_FactoryKey_LinearGradientPainter2D) {
		m_currentEntry = new Painter2DEditDialogLinearGradientEntry(_painter);
	}
	else if (_painter->getFactoryKey() == OT_FactoryKey_RadialGradientPainter2D) {
		m_currentEntry = new Painter2DEditDialogRadialGradientEntry(_painter);
	}
	else if (_painter->getFactoryKey() == OT_FactoryKey_StyleRefPainter2D) {
		m_currentEntry = new Painter2DEditDialogReferenceEntry(m_filter, _painter);
	}
	else {
		OT_LOG_E("Unknown painter type: \"" + _painter->getFactoryKey() + "\"");
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
