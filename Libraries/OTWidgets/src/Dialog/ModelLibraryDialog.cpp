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
#include "OTWidgets/ToolTipHandler.h"
#include "OTWidgets/Dialog/ModelLibraryDialog.h"
#include "OTWidgets/Widgets/Label.h"
#include "OTWidgets/Widgets/LineEdit.h"
#include "OTWidgets/Widgets/ComboBox.h"
#include "OTWidgets/Widgets/PushButton.h"
#include "OTWidgets/Widgets/ComboButton.h"

// Qt header
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qgroupbox.h>
#include <QtWidgets/qscrollarea.h>

ot::ModelLibraryDialog::ModelLibraryDialog(ModelLibraryDialogCfg&& _config, QWidget* _parent)
	: Dialog(_config, _parent), m_selectedOwner("")
{
	m_config = std::move(_config);

	// Create basic layout
	QVBoxLayout* rootLay = new QVBoxLayout(this);
	rootLay->setContentsMargins(0, 0, 0, 0);

	// Create filter layout FIRST (top)
	QGroupBox* filterWidget = new QGroupBox("Filter", this);
	QGridLayout* filterLay = new QGridLayout(filterWidget);
	rootLay->addWidget(filterWidget);

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

	// Title layout with Name and Source Selection
	QHBoxLayout* titleLay = new QHBoxLayout;
	rootLay->addLayout(titleLay);

	Label* titleLabel = new Label("Name:", this);
	titleLay->addWidget(titleLabel);

	m_sourceSelection = new ComboButton("All", this);

	// Fill the source selection
	m_sourceSelection->addItem(QString::fromStdString(LibraryModel::modelOriginToString(LibraryModel::ModelOrigin::Custom)));
	m_sourceSelection->addItem(QString::fromStdString(LibraryModel::modelOriginToString(LibraryModel::ModelOrigin::BuiltIn)));

	titleLay->addWidget(m_sourceSelection);

	m_nameEdit = new ComboBox(this);
	m_nameEdit->setEditable(true);
	titleLay->addWidget(m_nameEdit, 1);

	// Create scroll area for info (middle, expandable)
	QScrollArea* dataArea = new QScrollArea(this);
	dataArea->setWidgetResizable(true);
	dataArea->setFrameShape(QFrame::NoFrame);
	dataArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	dataArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	dataArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	rootLay->addWidget(dataArea, 1);

	QWidget* dataLayW = new QWidget(dataArea);
	dataArea->setWidget(dataLayW);

	QVBoxLayout* dataLay = new QVBoxLayout(dataLayW);
	dataLay->setContentsMargins(0, 0, 0, 0);

	m_infoGroup = new QGroupBox("Info", dataArea);
	m_infoLayout = new QGridLayout(m_infoGroup);
	m_infoLayout->setColumnStretch(1, 1);
	dataLay->addWidget(m_infoGroup);
	m_infoGroup->setHidden(true);

	dataLay->addStretch(1);

	// Create buttons (bottom)
	QHBoxLayout* buttonLay = new QHBoxLayout;
	rootLay->addLayout(buttonLay);
	PushButton* okButton = new PushButton("OK", this);
	PushButton* cancelButton = new PushButton("Cancel", this);
	buttonLay->addStretch(1);
	buttonLay->addWidget(okButton);
	buttonLay->addWidget(cancelButton);
	this->connect(okButton, &PushButton::clicked, this, &ModelLibraryDialog::slotConfirm);
	this->connect(cancelButton, &PushButton::clicked, this, &ModelLibraryDialog::closeCancel);

	// Source selection changed
	this->connect(m_sourceSelection, &ComboButton::selectedItemChanged, this, &ModelLibraryDialog::slotSourceSelectionChanged);

	// Initialize window
	this->connect(m_nameEdit, QOverload<int>::of(&ComboBox::currentIndexChanged), this, &ModelLibraryDialog::slotModelChanged);

	// Initialize models
	this->slotFilterChanged();
	this->slotSourceSelectionChanged();
}

ot::ModelLibraryDialog::~ModelLibraryDialog() {
}

// ###########################################################################################################################################################################################################################################################################################################################

// Slots

void ot::ModelLibraryDialog::slotConfirm() {
	const LibraryModel* selectedModel = this->getSelectedModel();

	if (selectedModel == nullptr) {
		ot::ToolTipHandler::showToolTip(m_nameEdit->mapToGlobal(QPoint(0, m_nameEdit->height())), "Please select a valid model");
		return;
	}

	m_selectedName = selectedModel->getName();
	m_selectedOwner = selectedModel->getOwner();
	this->closeOk();
}

void ot::ModelLibraryDialog::slotFilterChanged() {
	this->updateNameEdit();
}

void ot::ModelLibraryDialog::slotModelChanged() {
	for (QWidget* w : m_infoWidgets) {
		w->setVisible(false);
		m_infoLayout->removeWidget(w);
		w->deleteLater();
	}

	m_infoWidgets.clear();

	const LibraryModel* selectedModel = this->getSelectedModel();

	if (selectedModel == nullptr) {
		m_infoGroup->setVisible(false);
		return;
	}

	// Add info widgets
	int r = 0;
	for (const auto& metaData : selectedModel->getMetaData()) {
		Label* label = new Label(QString::fromStdString(metaData.first), this);
		LineEdit* edit = new LineEdit(QString::fromStdString(metaData.second), this);
		edit->setReadOnly(true);
		m_infoLayout->addWidget(label, r, 0);
		m_infoLayout->addWidget(edit, r, 1);
		m_infoWidgets.push_back(label);
		m_infoWidgets.push_back(edit);
		r++;
	}

	if (!selectedModel->getOwner().empty()) {
		// Add owner info
		Label* label = new Label("Owner", this);
		LineEdit* edit = new LineEdit(QString::fromStdString(selectedModel->getOwner()), this);
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
}

void ot::ModelLibraryDialog::slotSourceSelectionChanged()
{
	m_sourceFilteredModels.clear();
	std::string selectedSource = m_sourceSelection->text().toStdString();

	for (const LibraryModel& model : m_config.getModels()) {
		if (selectedSource != "All") {
			LibraryModel::ModelOrigin selectedOrigin =
				LibraryModel::stringToModelOrigin(selectedSource);
			if (model.getModelOrigin() != selectedOrigin) continue;
		}
		m_sourceFilteredModels.push_back(model);
	}
	this->updateNameEdit();
}

void ot::ModelLibraryDialog::updateNameEdit()
{
	// Store edit settings
	QString txt = m_nameEdit->currentText();
	m_nameEdit->clear();

	// Go through all models
	size_t itemIndex = 0;
	for (const LibraryModel& model : m_sourceFilteredModels) {
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
			// Display only the name, store index as data
			m_nameEdit->addItem(QString::fromStdString(model.getName()), static_cast<int>(itemIndex));
			itemIndex++;
		}
	}

	// Restore edit settings
	m_nameEdit->setCurrentText(txt);
}

const ot::LibraryModel* ot::ModelLibraryDialog::getSelectedModel() const {
	int selectedIndex = m_nameEdit->currentData().toInt();

	if (selectedIndex < 0 || selectedIndex >= static_cast<int>(m_sourceFilteredModels.size())) {
		return nullptr;
	}

	// Get iterator to the selected item
	auto it = m_sourceFilteredModels.begin();
	std::advance(it, selectedIndex);

	return &(*it);
}