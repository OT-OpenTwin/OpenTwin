// @otlicense
// File: ProjectOverviewFilter.cpp
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
#include "ProjectOverviewWidget.h"
#include "ProjectOverviewFilter.h"
#include "OTWidgets/Label.h"
#include "OTWidgets/LineEdit.h"
#include "OTWidgets/ToolButton.h"
#include "OTWidgets/PushButton.h"
#include "OTWidgets/IconManager.h"

// Qt header
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qlistwidget.h>

ot::ProjectOverviewFilter::ProjectOverviewFilter(ProjectOverviewWidget* _overview, int _logicalIndex, bool _sortOnly)
	: QMenu(_overview->getQWidget()), m_logicalIndex(_logicalIndex), m_isConfirmed(false),
	m_optionsList(nullptr), m_filterEdit(nullptr)
{
	QVBoxLayout* centralLayout = new QVBoxLayout(this);

	m_title = new Label(this);
	centralLayout->addWidget(m_title);

	QHBoxLayout* sortAscLayout = new QHBoxLayout;
	sortAscLayout->setContentsMargins(0, 0, 0, 0);
	centralLayout->addLayout(sortAscLayout);
	ToolButton* sortAscButton = new ToolButton(IconManager::getIcon("Button/SortAscending.png"), "", this);
	sortAscLayout->addWidget(sortAscButton);
	sortAscLayout->addWidget(new Label("Sort Ascending", this), 1);
	connect(sortAscButton, &ToolButton::clicked, this, &ProjectOverviewFilter::slotSortAscending);

	QHBoxLayout* sortDescLayout = new QHBoxLayout;
	sortDescLayout->setContentsMargins(0, 0, 0, 0);
	centralLayout->addLayout(sortDescLayout);
	ToolButton* sortDescButton = new ToolButton(IconManager::getIcon("Button/SortDescending.png"), "", this);
	sortDescLayout->addWidget(sortDescButton);
	sortDescLayout->addWidget(new Label("Sort Descending", this), 1);
	connect(sortDescButton, &ToolButton::clicked, this, &ProjectOverviewFilter::slotSortDescending);

	if (!_sortOnly) {
		m_filterEdit = new LineEdit(this);
		m_filterEdit->setPlaceholderText("Type to filter...");
		connect(m_filterEdit, &LineEdit::textChanged, this, &ProjectOverviewFilter::slotTextChanged);
		centralLayout->addWidget(m_filterEdit);

		m_optionsList = new QListWidget(this);
		m_optionsList->setMinimumSize(150, 150);
		m_optionsList->setSelectionMode(QAbstractItemView::NoSelection);
		m_optionsList->setSortingEnabled(false);
		setOptions(QStringList());
		connect(m_optionsList, &QListWidget::itemChanged, this, &ProjectOverviewFilter::slotCheckedChanged);
		centralLayout->addWidget(m_optionsList);
	}

	QHBoxLayout* buttonLayout = new QHBoxLayout;
	buttonLayout->setContentsMargins(0, 0, 0, 0);
	buttonLayout->addStretch();
	centralLayout->addLayout(buttonLayout);

	PushButton* confirmButton = new PushButton("Apply", this);
	connect(confirmButton, &PushButton::clicked, this, &ProjectOverviewFilter::slotConfirm);
	buttonLayout->addWidget(confirmButton);
	
	PushButton* cancelButton = new PushButton("Reset", this);
	connect(cancelButton, &PushButton::clicked, this, &ProjectOverviewFilter::slotCancel);
	buttonLayout->addWidget(cancelButton);
}

void ot::ProjectOverviewFilter::setTitle(const QString& _title) {
	m_title->setText(_title);
}

void ot::ProjectOverviewFilter::setOptions(const QStringList& _options) {
	if (m_optionsList) {
		QSignalBlocker blocker(m_optionsList);
		m_optionsList->clear();
		m_optionsList->addItem("< Select All >");
		m_optionsList->addItems(_options);

		for (int i = 0; i < m_optionsList->count(); ++i) {
			QListWidgetItem* item = m_optionsList->item(i);
			item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
			item->setCheckState(Qt::Unchecked);
		}
	}
}

ot::ProjectOverviewFilterData ot::ProjectOverviewFilter::getFilterData() const {
	ProjectOverviewFilterData data(m_logicalIndex);

	if (m_optionsList) {
		for (int i = 1; i < m_optionsList->count(); ++i) {
			QListWidgetItem* item = m_optionsList->item(i);
			if (item->checkState() == Qt::Checked) {
				data.addSelectedFilter(item->text());
			}
		}
	}

	return data;
}

void ot::ProjectOverviewFilter::setFromData(const ProjectOverviewFilterData& _data) {
	if (!_data.isValid() || _data.getLogicalIndex() != m_logicalIndex) {
		return;
	}

	if (m_optionsList) {
		QSignalBlocker blocker(m_optionsList);

		// Uncheck all
		for (int i = 0; i < m_optionsList->count(); ++i) {
			QListWidgetItem* item = m_optionsList->item(i);
			item->setCheckState(Qt::Unchecked);
		}

		// Check matching items
		for (int i = 1; i < m_optionsList->count(); ++i) {
			QListWidgetItem* item = m_optionsList->item(i);
			if (_data.getSelectedFilters().contains(item->text())) {
				item->setCheckState(Qt::Checked);
			}
		}

		// Update the "Select All" item
		bool allChecked = true;
		for (int i = 1; i < m_optionsList->count(); ++i) {
			QListWidgetItem* item = m_optionsList->item(i);
			if (item->checkState() != Qt::Checked) {
				allChecked = false;
				break;
			}
		}
		m_optionsList->item(0)->setCheckState((allChecked ? Qt::Checked : Qt::Unchecked));
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Slots

void ot::ProjectOverviewFilter::slotConfirm() {
	m_isConfirmed = true;
	close();
}

void ot::ProjectOverviewFilter::slotCancel() {
	close();
}

void ot::ProjectOverviewFilter::slotTextChanged() {
	OTAssertNullptr(m_optionsList);

	const QString filterText = m_filterEdit->text().toLower();

	for (int i = 1; i < m_optionsList->count(); i++) {
		QListWidgetItem* item = m_optionsList->item(i);
		const QString itemText = item->text().toLower();
		const bool shouldShow = itemText.contains(filterText) || filterText.isEmpty();
		item->setHidden(!shouldShow);
	}
}

void ot::ProjectOverviewFilter::slotSortAscending() {
	Q_EMIT sortOrderChanged(m_logicalIndex, ProjectOverviewFilterData::SortMode::Ascending);
}

void ot::ProjectOverviewFilter::slotSortDescending() {
	Q_EMIT sortOrderChanged(m_logicalIndex, ProjectOverviewFilterData::SortMode::Descending);
}

void ot::ProjectOverviewFilter::slotCheckedChanged(QListWidgetItem* _item) {
	OTAssertNullptr(m_optionsList);
	OTAssert(m_optionsList->count() > 0, "Options list is empty");

	QSignalBlocker blocker(m_optionsList);
	bool shouldAllSelect = false;
	if (_item == m_optionsList->item(0)) {
		shouldAllSelect = (_item->checkState() == Qt::Checked);
		for (int i = 1; i < m_optionsList->count(); ++i) {
			QListWidgetItem* item = m_optionsList->item(i);
			item->setCheckState(shouldAllSelect ? Qt::Checked : Qt::Unchecked);
		}
	}
	else {
		bool allChecked = true;
		for (int i = 1; i < m_optionsList->count(); ++i) {
			QListWidgetItem* item = m_optionsList->item(i);
			if (item->checkState() != Qt::Checked) {
				allChecked = false;
				break;
			}
		}

		m_optionsList->item(0)->setCheckState((allChecked ? Qt::Checked : Qt::Unchecked));
	}

	Q_EMIT filterChanged(this->getFilterData());
}
