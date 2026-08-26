// @otlicense

// OpenTwin header
#include "OTCore/Logging/Logger.h"
#include "OTWidgets/Widgets/Label.h"
#include "OTWidgets/Widgets/LineEdit.h"
#include "OTWidgets/Widgets/PushButton.h"
#include "OTWidgets/Widgets/JsonTreeWidget.h"
#include "Widgets/AdvancedJsonTree.h"

// Qt header
#include <QtCore/qfile.h>
#include <QtGui/qclipboard.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qfiledialog.h>
#include <QtWidgets/qscrollarea.h>
#include <QtWidgets/qapplication.h>

ot::AdvancedJsonTree::AdvancedJsonTree(QWidget* _parent)
	: QWidget(_parent), m_filterTimer(this)
{
	QVBoxLayout* layout = new QVBoxLayout(this);
	QScrollArea* serviceArea = new QScrollArea(this);
	layout->addWidget(serviceArea, 1);
	serviceArea->setWidgetResizable(true);
	m_tree = new ot::JsonTreeWidget(serviceArea);
	m_tree->setReadOnly(true);
	serviceArea->setWidget(m_tree);
	connect(m_tree, &ot::JsonTreeWidget::nodeDoubleClicked, this, &AdvancedJsonTree::slotNodeDoubleClicked);

	QHBoxLayout* controlLayout = new QHBoxLayout;
	layout->addLayout(controlLayout);

	ot::PushButton* expandButton = new ot::PushButton("Expand All", this);
	controlLayout->addWidget(expandButton);
	connect(expandButton, &ot::PushButton::clicked, m_tree, &ot::JsonTreeWidget::expandAll);

	ot::PushButton* collapseButton = new ot::PushButton("Collapse All", this);
	controlLayout->addWidget(collapseButton);
	connect(collapseButton, &ot::PushButton::clicked, m_tree, &ot::JsonTreeWidget::collapseAll);

	controlLayout->addWidget(new Label("Filter:", this));
	m_textFilter = new ot::LineEdit(this);
	m_textFilter->setPlaceholderText("Filter...");
	m_textFilter->setMinimumWidth(150);
	controlLayout->addWidget(m_textFilter);
	connect(m_textFilter, &ot::LineEdit::textChanged, this, &AdvancedJsonTree::slotFilterTextChanged);

	m_statusLabel = new ot::Label(this);
	controlLayout->addWidget(m_statusLabel);

	controlLayout->addStretch(1);

	ot::PushButton* exportButton = new ot::PushButton("Export", this);
	controlLayout->addWidget(exportButton);
	connect(exportButton, &ot::PushButton::clicked, this, &AdvancedJsonTree::exportToFile);

	m_filterTimer.setInterval(500);
	m_filterTimer.setSingleShot(true);
	connect(&m_filterTimer, &QTimer::timeout, this, &AdvancedJsonTree::applyCurrentFilter);
}

void ot::AdvancedJsonTree::setFromJsonDocument(const ot::JsonDocument& _doc)
{
	{
		m_tree->setFromJsonDocument(_doc);
	}
	{
		//m_tree->countItems(m_status.numberOfItems, m_status.numberOfHiddenItems);
	}

	//updateStatus();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Slots

void ot::AdvancedJsonTree::applyCurrentFilter()
{
	OTAssertNullptr(m_tree);
	m_tree->filterItems(m_textFilter->text());
}

void ot::AdvancedJsonTree::exportToFile()
{
	ot::JsonDocument doc = m_tree->toJsonDocument();
	QByteArray jsonString = QByteArray::fromStdString(doc.toJson());
	if (jsonString.length() < 3)
	{
		return;
	}

	QString fileName = QFileDialog::getSaveFileName(this, "Export Debug Info", "", "JSON Files (*.json);;All Files (*)");
	if (fileName.isEmpty())
	{
		return;
	}

	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly))
	{
		OT_LOG_ES("Could not open file for writing: \"" << fileName.toStdString());
		return;
	}

	file.write(jsonString);
	file.close();
}

void ot::AdvancedJsonTree::slotFilterTextChanged()
{
	m_filterTimer.stop();

	switch (m_status.filterMode)
	{
	case FilterMode::OnChange:
		this->applyCurrentFilter();
		break;

	case FilterMode::AfterDelay:
		m_filterTimer.start();
		break;

	default:
		OT_LOG_ES("Unknown filter mode (" << (int)m_status.filterMode);

	}
}

void ot::AdvancedJsonTree::slotNodeDoubleClicked(int _column, ot::JsonTreeWidgetNode* _node)
{
	switch (_column)
	{
	case ot::JsonTreeWidgetModel::ColumnValue:
		QApplication::clipboard()->setText(_node->getValue());
		OT_LOG_E("Copied value to clipboard: " + _node->getValue().toStdString());
		break;

	default:
		break;
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Helper

bool ot::AdvancedJsonTree::statesDiffer(bool _visualChangesOnly) const
{
	return
		m_status.numberOfItems != m_lastStatus.numberOfItems ||
		m_status.numberOfHiddenItems != m_lastStatus.numberOfHiddenItems ||
		(!_visualChangesOnly &&
			m_status.filterMode != m_lastStatus.filterMode
		);
}

void ot::AdvancedJsonTree::updateStatus()
{
	if (!statesDiffer(true))
	{
		return;
	}

	const int visItems = m_status.numberOfItems - m_status.numberOfHiddenItems;
	QString statusText;

	if (visItems == m_status.numberOfItems)
	{
		statusText = QString::number(m_status.numberOfItems) + (m_status.numberOfItems == 1 ? QString(" item") : QString(" items"));
	}
	else
	{
		statusText = QString::number(visItems) % QString(" (of ") % QString::number(m_status.numberOfItems) % QString(") items");
	}

	m_statusLabel->setText(statusText);

	m_lastStatus = m_status;
}