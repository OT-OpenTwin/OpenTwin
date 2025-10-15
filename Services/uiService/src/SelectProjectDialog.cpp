//! @file SelectProjectDialog.cpp
//! @authors Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

// Frontend header
#include "AppBase.h"
#include "ProjectManagement.h"
#include "SelectProjectDialog.h"

// OpenTwin header
#include "OTWidgets/Label.h"
#include "OTWidgets/LineEdit.h"
#include "OTWidgets/PushButton.h"
#include "OTWidgets/IconManager.h"

// Qt header
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qlistwidget.h>

SelectProjectDialogEntry::SelectProjectDialogEntry(const ot::ProjectInformation& _info) :
	m_info(_info)
{
	this->setText(QString::fromStdString(m_info.getProjectName()));

	QFont font = this->font();
	font.setPixelSize(16);
	this->setFont(font);
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

SelectProjectDialog::SelectProjectDialog(const ot::DialogCfg& _config)
	: Dialog(_config)
{
	// Setup widgets
	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	QVBoxLayout* centralLayout = new QVBoxLayout;
	QHBoxLayout* bottomLayout = new QHBoxLayout;

	m_filter = new ot::LineEdit(this);
	m_filter->setPlaceholderText("Filter projects (Confirm with Return)...");
	centralLayout->addWidget(m_filter);

	m_list = new QListWidget(this);
	m_list->setSelectionMode(QAbstractItemView::SingleSelection);
	m_list->setIconSize(QSize(32, 32));

	centralLayout->addWidget(m_list, 1);

	mainLayout->addLayout(centralLayout, 1);

	m_infoLabel = new ot::Label(this);

	m_confirmButton = new ot::PushButton("Confirm", this);
	m_confirmButton->setEnabled(false);
	ot::PushButton* cancelButton = new ot::PushButton("Cancel", this);

	bottomLayout->addWidget(m_infoLabel, 1);
	bottomLayout->addWidget(m_confirmButton);
	bottomLayout->addWidget(cancelButton);
	mainLayout->addLayout(bottomLayout);

	connect(m_confirmButton, &QPushButton::clicked, this, &SelectProjectDialog::slotConfirm);
	connect(cancelButton, &QPushButton::clicked, this, &SelectProjectDialog::closeCancel);
	connect(m_list, &QListWidget::itemDoubleClicked, this, &SelectProjectDialog::slotItemDoubleClicked);
	connect(m_list, &QListWidget::itemSelectionChanged, this, &SelectProjectDialog::slotSelectionChanged);
	connect(m_filter, &ot::LineEdit::editingFinished, this, &SelectProjectDialog::slotRefillList);

	// Initialize data
	AppBase* app = AppBase::instance();

	for (const auto& it : app->getProjectTypeDefaultIconNameMap()) {
		m_projectTypeToIconMap.emplace(it.first, ot::IconManager::instance().getIcon(QString::fromStdString(it.second)));
	}

	this->slotRefillList();
}

SelectProjectDialog::~SelectProjectDialog() {
}

ot::ProjectInformation SelectProjectDialog::getSelectedProject() const {
	auto sel = m_list->selectedItems();
	if (sel.size() != 1) {
		return ot::ProjectInformation();
	}
	else {
		SelectProjectDialogEntry* entry = dynamic_cast<SelectProjectDialogEntry*>(sel.front());
		if (!entry) {
			OT_LOG_E("Unexpected item type");
			return ot::ProjectInformation();
		}
		return entry->getProjectInformation();
	}
}

void SelectProjectDialog::slotConfirm() {
	auto sel = m_list->selectedItems();
	if (sel.size() != 1) {
		return;
	}
	else {
		this->closeOk();
	}
}

void SelectProjectDialog::slotItemDoubleClicked(QListWidgetItem* _item) {
	QSignalBlocker blocker(m_list);
	m_list->clearSelection();
	_item->setSelected(true);
	this->slotConfirm();
}

void SelectProjectDialog::slotSelectionChanged() {
	m_confirmButton->setEnabled(!m_list->selectedItems().empty());
}

void SelectProjectDialog::slotRefillList() {
	// Store current selection
	std::string currentSelection;
	auto sel = m_list->selectedItems();
	if (sel.size() == 1) {
		SelectProjectDialogEntry* entry = dynamic_cast<SelectProjectDialogEntry*>(sel.front());
		if (entry) {
			currentSelection = entry->getProjectInformation().getProjectName();
		}
		else {
			OT_LOG_E("Unexpected item type");
		}
	}

	QSignalBlocker blocker(m_list);
	m_list->clear();

	// Get project list
	AppBase* app = AppBase::instance();
	ProjectManagement manager(app->getCurrentLoginData());

	const int maxNumberOfResults = 1000;

	std::list<ot::ProjectInformation> projects;
	bool maxLengthExceeded = false;
	manager.findProjects(m_filter->text().toStdString(), maxNumberOfResults, projects, maxLengthExceeded);

	// Refill list
	for (const ot::ProjectInformation& proj : projects) {
		SelectProjectDialogEntry* entry = new SelectProjectDialogEntry(proj);
		auto icoIt = m_projectTypeToIconMap.find(proj.getProjectType());
		if (icoIt != m_projectTypeToIconMap.end()) {
			entry->setIcon(icoIt->second);
		}
		else {
			entry->setIcon(app->getDefaultProjectTypeIcon());
		}

		m_list->addItem(entry);
		if (entry->getProjectInformation().getProjectName() == currentSelection) {
			entry->setSelected(true);
		}
	}

	// Update info label
	if (maxLengthExceeded) {
		m_infoLabel->setText("More than " + QString::number(maxNumberOfResults) + " projects found");
	}
	else if (projects.size() == 1) {
		m_infoLabel->setText(QString::number(projects.size()) + " project found.");
	}
	else {
		m_infoLabel->setText(QString::number(projects.size()) + " projects found.");
	}

	// Refresh selection state
	this->slotSelectionChanged();
}
