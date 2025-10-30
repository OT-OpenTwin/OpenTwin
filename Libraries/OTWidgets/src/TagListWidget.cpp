// @otlicense
// File: TagListWidget.cpp
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
#include "OTWidgets/Label.h"
#include "OTWidgets/ComboBox.h"
#include "OTWidgets/FlowLayout.h"
#include "OTWidgets/ToolButton.h"
#include "OTWidgets/IconManager.h"
#include "OTWidgets/TagListWidget.h"

namespace ot {

	TagListEntry::TagListEntry(QWidget* _parent) : QFrame(_parent) {
		setFrameShape(QFrame::Box);
		setFrameShadow(QFrame::Raised);
		QHBoxLayout* layout = new QHBoxLayout(this);
		layout->setContentsMargins(2, 2, 2, 2);
		layout->setSpacing(2);

		m_text = new Label(this);
		layout->addWidget(m_text, 1);

		m_removeButton = new ToolButton(this);
		m_removeButton->setFixedSize(16, 16);
		m_removeButton->setIcon(IconManager::getIcon("Button/Remove.png"));
		layout->addWidget(m_removeButton);
		connect(m_removeButton, &ToolButton::clicked, this, &TagListEntry::slotRemove);
	}

	TagListEntry::~TagListEntry() {

	}

	void TagListEntry::setText(const QString& _text) {
		m_text->setText(_text);
	}

	QString TagListEntry::getText() const {
		return m_text->text();
	}

	void TagListEntry::slotRemove() {
		Q_EMIT removeRequested(m_text->text());
	}

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	TagListWidget::TagListWidget(QWidget* _parent)
		: QFrame(_parent) 
	{
		QVBoxLayout* mainLayout = new QVBoxLayout(this);

		QFrame* tagFrame = new QFrame(this);
		tagFrame->setFrameShape(QFrame::Box);
		tagFrame->setObjectName("TagListWidgetTagFrame");
		m_layout = new FlowLayout(tagFrame);
		mainLayout->addWidget(tagFrame, 1);

		m_input = new ComboBox(this);
		m_input->setEditable(true);
		mainLayout->addWidget(m_input);
		connect(m_input, &ComboBox::returnPressed, this, &TagListWidget::slotAddTagFromInput);

		setFrameShape(QFrame::NoFrame);
	}

	TagListWidget::~TagListWidget() {

	}

	void TagListWidget::addTag(const QString& _tag) {
		for (const TagListEntry* entry : m_tags) {
			if (entry->getText() == _tag) {
				return;
			}
		}

		TagListEntry* entry = new TagListEntry(this);
		entry->setText(_tag);
		m_layout->addWidget(entry);
		connect(entry, &TagListEntry::removeRequested, this, &TagListWidget::removeTag);
		m_tags.push_back(entry);
		addOption(_tag);

		Q_EMIT tagsChanged();
	}

	void TagListWidget::setTags(const QStringList& _tags) {
		{
			QSignalBlocker blocker(this);
			clear();
			for (const QString& tag : _tags) {
				addTag(tag);
			}
		}

		Q_EMIT tagsChanged();
	}

	QStringList TagListWidget::getTags() const {
		QStringList lst;
		for (const TagListEntry* entry : m_tags) {
			lst.append(entry->getText());
		}
		return lst;
	}

	void TagListWidget::clear() {
		for (TagListEntry* entry : m_tags) {
			disconnect(entry, &TagListEntry::removeRequested, this, &TagListWidget::removeTag);
			entry->setVisible(false);
			m_layout->removeWidget(entry);
			entry->deleteLater();
		}
		m_tags.clear();
		m_input->clear();

		Q_EMIT tagsChanged();
	}

	void TagListWidget::addOption(const QString& _option) {
		for (int i = 0; i < m_input->count(); i++) {
			if (m_input->itemText(i) == _option) {
				return;
			}
		}
		m_input->addItem(_option);
	}

	void TagListWidget::addOptions(const QStringList& _options) {
		for (const QString& option : _options) {
			addOption(option);
		}
	}

	void TagListWidget::setEditable(bool _editable) {
		m_input->setEditable(_editable);
	}

	bool TagListWidget::isEditable() const {
		return m_input->isEditable();
	}

	void TagListWidget::setPlaceholderText(const QString& _text) {
		m_input->setPlaceholderText(_text);
	}

	QString TagListWidget::getPlaceholderText() const {
		return m_input->placeholderText();
	}

	void TagListWidget::removeTag(const QString& _tag) {
		for (auto it = m_tags.begin(); it != m_tags.end(); it++) {
			if ((*it)->getText() == _tag) {
				TagListEntry* entry = *it;
				disconnect(entry, &TagListEntry::removeRequested, this, &TagListWidget::removeTag);
				entry->setVisible(false);
				m_layout->removeWidget(entry);
				entry->deleteLater();
				m_tags.erase(it);

				Q_EMIT tagsChanged();
				break;
			}
		}
	}

	void TagListWidget::slotAddTagFromInput() {
		if (!m_input->currentText().isEmpty()) {
			addTag(m_input->currentText());
			m_input->setCurrentText("");
		}
	}

}