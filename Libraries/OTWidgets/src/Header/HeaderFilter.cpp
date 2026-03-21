// @otlicense
// File: HeaderFilter.cpp
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
#include "OTWidgets/Positioning.h"
#include "OTWidgets/Style/IconManager.h"
#include "OTWidgets/Header/HeaderFilter.h"
#include "OTWidgets/Widgets/Label.h"
#include "OTWidgets/Widgets/LineEdit.h"
#include "OTWidgets/Widgets/ToolButton.h"
#include "OTWidgets/Widgets/PushButton.h"

// Qt header
#include <QtGui/qscreen.h>
#include <QtGui/qguiapplication.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qlistwidget.h>

ot::HeaderFilter::HeaderFilter(int _logicalIndex, bool _sortOnly, QWidget* _parent)
	: QMenu(_parent), m_logicalIndex(_logicalIndex), m_isConfirmed(false),
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
	connect(sortAscButton, &ToolButton::clicked, this, &HeaderFilter::slotSortAscending);

	QHBoxLayout* sortDescLayout = new QHBoxLayout;
	sortDescLayout->setContentsMargins(0, 0, 0, 0);
	centralLayout->addLayout(sortDescLayout);
	ToolButton* sortDescButton = new ToolButton(IconManager::getIcon("Button/SortDescending.png"), "", this);
	sortDescLayout->addWidget(sortDescButton);
	sortDescLayout->addWidget(new Label("Sort Descending", this), 1);
	connect(sortDescButton, &ToolButton::clicked, this, &HeaderFilter::slotSortDescending);

	if (!_sortOnly)
	{
		m_filterEdit = new LineEdit(this);
		m_filterEdit->setPlaceholderText("Type to filter...");
		connect(m_filterEdit, &LineEdit::textChanged, this, &HeaderFilter::slotTextChanged);
		centralLayout->addWidget(m_filterEdit);

		m_optionsList = new QListWidget(this);
		m_optionsList->setMinimumSize(150, 150);
		m_optionsList->setSelectionMode(QAbstractItemView::NoSelection);
		m_optionsList->setSortingEnabled(false);
		setOptions(QStringList());
		connect(m_optionsList, &QListWidget::itemChanged, this, &HeaderFilter::slotCheckedChanged);
		centralLayout->addWidget(m_optionsList);

		QHBoxLayout* buttonLayout = new QHBoxLayout;
		buttonLayout->setContentsMargins(0, 0, 0, 0);
		buttonLayout->addStretch();
		centralLayout->addLayout(buttonLayout);

		PushButton* confirmButton = new PushButton("Apply", this);
		connect(confirmButton, &PushButton::clicked, this, &HeaderFilter::slotConfirm);
		buttonLayout->addWidget(confirmButton);

		PushButton* cancelButton = new PushButton("Cancel", this);
		connect(cancelButton, &PushButton::clicked, this, &HeaderFilter::slotCancel);
		buttonLayout->addWidget(cancelButton);
	}
}

void ot::HeaderFilter::setTitle(const QString& _title)
{
	m_title->setText(_title);
}

void ot::HeaderFilter::setOptions(const QStringList& _options)
{
	if (m_optionsList)
	{
		QSignalBlocker blocker(m_optionsList);
		m_optionsList->clear();
		m_optionsList->addItem("< Select All >");
		m_optionsList->addItems(_options);

		for (int i = 0; i < m_optionsList->count(); ++i)
		{
			QListWidgetItem* item = m_optionsList->item(i);
			item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
			item->setCheckState(Qt::Unchecked);
		}
	}
}

void ot::HeaderFilter::updateCheckedState(const QStringList& _checkedItems)
{
	if (m_optionsList)
	{
		QSignalBlocker blocker(m_optionsList);

		if (_checkedItems.empty())
		{
			// No filters provided, check all
			for (int i = 0; i < m_optionsList->count(); ++i)
			{
				QListWidgetItem* item = m_optionsList->item(i);
				item->setCheckState(Qt::Checked);
			}
		}
		else
		{
			// Uncheck all
			for (int i = 0; i < m_optionsList->count(); ++i)
			{
				QListWidgetItem* item = m_optionsList->item(i);
				item->setCheckState(Qt::Unchecked);
			}

			// Check matching items
			bool allChecked = true;
			for (int i = 1; i < m_optionsList->count(); ++i)
			{
				QListWidgetItem* item = m_optionsList->item(i);
				if (_checkedItems.contains(item->text()))
				{
					item->setCheckState(Qt::Checked);
				}
				else
				{
					allChecked = false;
				}
			}

			// Update the "Select All" item
			m_optionsList->item(0)->setCheckState((allChecked ? Qt::Checked : Qt::Unchecked));
		}
	}
}

QStringList ot::HeaderFilter::saveCheckedState() const
{
	QStringList data;

	if (m_optionsList)
	{
		bool hasUnchecked = false;
		for (int i = 1; i < m_optionsList->count(); ++i)
		{
			QListWidgetItem* item = m_optionsList->item(i);
			if (item->checkState() == Qt::Checked)
			{
				data.append(item->text());
			}
			else
			{
				hasUnchecked = true;
			}
		}

		// If all items are checked, clear the selection to indicate no filtering
		if (!hasUnchecked)
		{
			data.clear();
		}
	}

	return data;
}

void ot::HeaderFilter::showAt(const QPoint& _pos)
{
	layout()->activate();
	QRect menuRect(_pos, layout()->totalSizeHint());

	menuRect = Positioning::fitOnScreen(menuRect, Positioning::FitByTopLeft);
	exec(menuRect.topLeft());
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Slots

void ot::HeaderFilter::slotConfirm()
{
	m_isConfirmed = true;
	close();
}

void ot::HeaderFilter::slotCancel()
{
	close();
}

void ot::HeaderFilter::slotTextChanged()
{
	OTAssertNullptr(m_optionsList);

	const QString filterText = m_filterEdit->text().toLower();

	for (int i = 1; i < m_optionsList->count(); i++)
	{
		QListWidgetItem* item = m_optionsList->item(i);
		const QString itemText = item->text().toLower();
		const bool shouldShow = itemText.contains(filterText) || filterText.isEmpty();
		item->setHidden(!shouldShow);
	}
}

void ot::HeaderFilter::slotSortAscending()
{
	Q_EMIT sortOrderChanged(m_logicalIndex, Qt::AscendingOrder);
}

void ot::HeaderFilter::slotSortDescending()
{
	Q_EMIT sortOrderChanged(m_logicalIndex, Qt::DescendingOrder);
}

void ot::HeaderFilter::slotCheckedChanged(QListWidgetItem* _item)
{
	OTAssertNullptr(m_optionsList);
	OTAssert(m_optionsList->count() > 0, "Options list is empty");

	QSignalBlocker blocker(m_optionsList);
	bool shouldAllSelect = false;
	if (_item == m_optionsList->item(0))
	{
		shouldAllSelect = (_item->checkState() == Qt::Checked);
		for (int i = 1; i < m_optionsList->count(); ++i)
		{
			QListWidgetItem* item = m_optionsList->item(i);
			item->setCheckState(shouldAllSelect ? Qt::Checked : Qt::Unchecked);
		}
	}
	else
	{
		bool allChecked = true;
		for (int i = 1; i < m_optionsList->count(); ++i)
		{
			QListWidgetItem* item = m_optionsList->item(i);
			if (item->checkState() != Qt::Checked)
			{
				allChecked = false;
				break;
			}
		}

		m_optionsList->item(0)->setCheckState((allChecked ? Qt::Checked : Qt::Unchecked));
	}
}
