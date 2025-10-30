// @otlicense

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
		FlowLayout* layout = new FlowLayout(this);
		layout->setContentsMargins(2, 2, 2, 2);
		layout->setSpacing(2);

		m_text = new Label(this);
		layout->addWidget(m_text);

		m_removeButton = new ToolButton(this);
		m_removeButton->setFixedSize(12, 12);
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

		m_layout = new FlowLayout;
		m_layout->setParent(this);
		mainLayout->addLayout(m_layout);

		m_input = new ComboBox(this);
		m_input->setEditable(true);
		mainLayout->addWidget(m_input);
		connect(m_input, &ComboBox::returnPressed, this, &TagListWidget::slotAddTagFromInput);
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

		Q_EMIT tagsChanged();
	}

	void TagListWidget::setTags(const QStringList& _tags) {
		{
			QSignalBlocker blocker(this);
			clearTags();
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

	void TagListWidget::clearTags() {
		for (TagListEntry* entry : m_tags) {
			disconnect(entry, &TagListEntry::removeRequested, this, &TagListWidget::removeTag);
			entry->setVisible(false);
			m_layout->removeWidget(entry);
			entry->deleteLater();
		}
		m_tags.clear();

		Q_EMIT tagsChanged();
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