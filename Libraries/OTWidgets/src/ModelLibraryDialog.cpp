// @otlicense
// File: ModelLibraryDialog.cpp
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
#include "OTCore/String.h"
#include "OTWidgets/Label.h"
#include "OTWidgets/LineEdit.h"
#include "OTWidgets/ComboBox.h"
#include "OTWidgets/PushButton.h"
#include "OTWidgets/ToolTipHandler.h"
#include "OTWidgets/ModelLibraryDialog.h"

// Qt header
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qgroupbox.h>
#include <QtWidgets/qscrollarea.h>

ot::ModelLibraryDialog::ModelLibraryDialog(ModelLibraryDialogCfg&& _config)
	: Dialog(_config)
{
	m_config = std::move(_config);

	// Create basic layout
	QVBoxLayout* rootLay = new QVBoxLayout(this);
	rootLay->setContentsMargins(0, 0, 0, 0);

	QHBoxLayout* titleLay = new QHBoxLayout;
	rootLay->addLayout(titleLay);
	
	Label* titleLabel = new Label("Name:", this);
	titleLay->addWidget(titleLabel);

	m_nameEdit = new ComboBox(this);
	m_nameEdit->setEditable(true);
	titleLay->addWidget(m_nameEdit, 1);

	// Create filter layout
	QScrollArea* dataArea = new QScrollArea(this);
	dataArea->setWidgetResizable(true);
	dataArea->setFrameShape(QFrame::NoFrame);
	dataArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	dataArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	dataArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	rootLay->addWidget(dataArea);

	QWidget* dataLayW = new QWidget(dataArea);
	dataArea->setWidget(dataLayW);

	QVBoxLayout* dataLay = new QVBoxLayout(dataLayW);
	dataLay->setContentsMargins(0, 0, 0, 0);

	QGroupBox* filterWidget = new QGroupBox("Filter", dataArea);
	QGridLayout* filterLay = new QGridLayout(filterWidget);
	dataLay->addWidget(filterWidget);

	// Create filters
	int r = 0;
	for (const std::string& filter : m_config.getFilters()) {
		FilterInputEntry entry;
		entry.label = new Label(QString::fromStdString(filter), filterWidget);
		entry.edit = new LineEdit(filterWidget);
		entry.name = filter;

		filterLay->addWidget(entry.label, r, 0);
		filterLay->addWidget(entry.edit, r, 1);
		r++;

		this->connect(entry.edit, &LineEdit::textChanged, this, &ModelLibraryDialog::slotFilterChanged);
		m_filterEntries.push_back(std::move(entry));
	}

	if (r == 0) {
		filterWidget->setVisible(false);
	}
	else {
		filterLay->setRowStretch(r, 1);
		filterLay->setColumnStretch(1, 1);
	}

	m_infoGroup = new QGroupBox("Info", dataArea);

	m_infoLayout = new QGridLayout(m_infoGroup);
	m_infoLayout->setColumnStretch(1, 1);
	dataLay->addWidget(m_infoGroup);
	m_infoGroup->setHidden(true);

	dataLay->addStretch(1);

	// Create buttons
	QHBoxLayout* buttonLay = new QHBoxLayout;
	rootLay->addLayout(buttonLay);
	PushButton* okButton = new PushButton("OK", this);
	PushButton* cancelButton = new PushButton("Cancel", this);
	buttonLay->addStretch(1);
	buttonLay->addWidget(okButton);
	buttonLay->addWidget(cancelButton);
	this->connect(okButton, &PushButton::clicked, this, &ModelLibraryDialog::slotConfirm);
	this->connect(cancelButton, &PushButton::clicked, this, &ModelLibraryDialog::closeCancel);

	// Initialize window
	this->connect(m_nameEdit, &ComboBox::currentTextChanged, this, &ModelLibraryDialog::slotModelChanged);

	// Initialize models
	this->slotFilterChanged();
}

ot::ModelLibraryDialog::~ModelLibraryDialog() {
}

// ###########################################################################################################################################################################################################################################################################################################################

// Slots

void ot::ModelLibraryDialog::slotConfirm() {
	// Ensure name is valid
	std::string name = m_nameEdit->currentText().toStdString();
	std::string lowerName = String::toLower(name);

	if (name.empty()) {
		ot::ToolTipHandler::showToolTip(m_nameEdit->mapToGlobal(QPoint(0, m_nameEdit->height())), "Please select a model name");
		return;
	}

	for (const LibraryModel& model : m_config.getModels()) {
		if (String::toLower(model.getName()) == lowerName) {
			m_selectedName = model.getName();
			this->closeOk();
			return;
		}
	}

	// No valid model found
	ot::ToolTipHandler::showToolTip(m_nameEdit->mapToGlobal(QPoint(0, m_nameEdit->height())), "Invalid model name selected");
}

void ot::ModelLibraryDialog::slotFilterChanged() {
	// Store edit settings
	QString txt = m_nameEdit->currentText();

	m_nameEdit->clear();

	// Go trough all models
	for (const LibraryModel& model : m_config.getModels()) {
		bool match = true;

		// Check all filters
		for (const FilterInputEntry& entry : m_filterEntries) {
			if (!entry.edit->text().isEmpty()) {
				// Ensure model has the data
				if (model.hasMetaDataValue(entry.name)) {
					const std::string metaDataValue = String::toLower(model.getMetaDataValue(entry.name));
					const std::string filterValue = String::toLower(entry.edit->text().toStdString());
					if (metaDataValue.find(filterValue) == std::string::npos) {
						match = false;
						break;
					}
				}
			}
		}

		if (match) {
			m_nameEdit->addItem(QString::fromStdString(model.getName()));
		}
	}

	// Restore edit settings
	m_nameEdit->setCurrentText(txt);
}

void ot::ModelLibraryDialog::slotModelChanged() {
	for (QWidget* w : m_infoWidgets) {
		w->setVisible(false);
		m_infoLayout->removeWidget(w);
		delete w;
	}

	m_infoWidgets.clear();

	// Find model
	std::string nameLower = String::toLower(m_nameEdit->currentText().toStdString());
	if (nameLower.empty()) {
		return;
	}

	m_infoGroup->setVisible(false);

	for (const LibraryModel& model : m_config.getModels()) {
		if (String::toLower(model.getName()) == nameLower) {
			// Add info widgets
			int r = 0;
			for (const auto& metaData : model.getMetaData()) {
				Label* label = new Label(QString::fromStdString(metaData.first), this);
				LineEdit* edit = new LineEdit(QString::fromStdString(metaData.second), this);
				edit->setReadOnly(true);
				m_infoLayout->addWidget(label, r, 0);
				m_infoLayout->addWidget(edit, r, 1);
				m_infoWidgets.push_back(label);
				m_infoWidgets.push_back(edit);
				r++;
			}
			m_infoLayout->setRowStretch(r, 1);

			if (r > 0) {
				m_infoGroup->setVisible(true);
			}

			break;
		}
	}
}
