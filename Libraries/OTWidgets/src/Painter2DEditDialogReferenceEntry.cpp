// @otlicense
// File: Painter2DEditDialogReferenceEntry.cpp
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
