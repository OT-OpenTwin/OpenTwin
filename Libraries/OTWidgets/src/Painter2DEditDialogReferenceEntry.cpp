//! @file Painter2DEditDialogReferenceEntry.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/StyleRefPainter2D.h"
#include "OTWidgets/ComboBox.h"
#include "OTWidgets/Painter2DEditDialogReferenceEntry.h"

ot::Painter2DEditDialogReferenceEntry::Painter2DEditDialogReferenceEntry(const Painter2DDialogFilter& _filter, const Painter2D* _painter) {
	m_comboBox = new ComboBox;

	const StyleRefPainter2D* actualPainter = dynamic_cast<const StyleRefPainter2D*>(_painter);

	// Determine available options
	QStringList optionList;
	for (ColorStyleValueEntry opt : _filter.getStyleReferences()) {
		optionList.append(QString::fromStdString(ot::toString(opt)));
	}

	optionList.sort();
	m_comboBox->addItems(optionList);

	// Determine initial text and valid options
	QString txt;
	if (actualPainter) {
		txt = QString::fromStdString(toString(actualPainter->getReferenceKey()));
	}
	else if (!optionList.isEmpty()) {
		txt = optionList.first();
	}
	else {
		OT_LOG_W("No color style references whitelisted. Disabling selection...");
		m_comboBox->setEnabled(false);
	}

	// Finalize
	m_comboBox->setEditable(false);
	m_comboBox->setCurrentText(txt);

	// Connect signals
	this->connect(m_comboBox, &ComboBox::currentTextChanged, this, &Painter2DEditDialogReferenceEntry::slotValueChanged);
}

ot::Painter2DEditDialogReferenceEntry::~Painter2DEditDialogReferenceEntry() {
	delete m_comboBox;
}

QWidget* ot::Painter2DEditDialogReferenceEntry::getRootWidget() const {
	return m_comboBox;
}

ot::Painter2D* ot::Painter2DEditDialogReferenceEntry::createPainter() const {
	StyleRefPainter2D* newPainter = new StyleRefPainter2D;
	newPainter->setReferenceKey(stringToColorStyleValueEntry(m_comboBox->currentText().toStdString()));

	return newPainter;
}
