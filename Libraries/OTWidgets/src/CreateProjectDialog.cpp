//! @file CreateProjectDialog.cpp
//! @author Alexander Kuester (alexk95)
//! @date September 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/Label.h"
#include "OTWidgets/LineEdit.h"
#include "OTWidgets/PushButton.h"
#include "OTWidgets/IconManager.h"
#include "OTWidgets/CreateProjectDialog.h"

// Qt header
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qsplitter.h>
#include <QtWidgets/qlistwidget.h>
#include <QtWidgets/qscrollarea.h>
#include <QtWidgets/qmessagebox.h>

#define OT_CREATEPROJECTDIALOG_TitlePrefix "Create Project"

ot::CreateProjectDialog::CreateProjectDialog(QWidget* _parentWidget)
	: Dialog(_parentWidget)
{
	// Create layouts
	QVBoxLayout* centralLayout = new QVBoxLayout(this);
	QHBoxLayout* nameAndButtonLayout = new QHBoxLayout;

	// Create controls
	QSplitter* centralSplitter = new QSplitter(Qt::Horizontal);

	m_search = new LineEdit;
	m_list = new QListWidget;

	QScrollArea* infoScrollArea = new QScrollArea;
	m_info = new Label;
	m_info->setWordWrap(true);
	infoScrollArea->setWidget(m_info);
	infoScrollArea->setWidgetResizable(true);
	infoScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	Label* nameLabel = new Label("Project Name:");
	m_name = new LineEdit;

	m_createButton = new PushButton("Create");
	PushButton* cancelButton = new PushButton("Cancel");

	// Setup controls
	m_search->setPlaceholderText("Search...");
	m_list->setIconSize(QSize(32, 32));
	m_info->setMinimumWidth(100);
	m_info->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	m_createButton->setEnabled(false);

	// Setup layouts
	centralLayout->addWidget(m_search);
	centralLayout->addWidget(centralSplitter);
	centralLayout->addLayout(nameAndButtonLayout);

	centralSplitter->addWidget(m_list);
	centralSplitter->addWidget(infoScrollArea);

	nameAndButtonLayout->addWidget(nameLabel);
	nameAndButtonLayout->addWidget(m_name, 1);
	nameAndButtonLayout->addWidget(m_createButton);
	nameAndButtonLayout->addWidget(cancelButton);

	// Setup dialog
	this->setWindowTitle(OT_CREATEPROJECTDIALOG_TitlePrefix);
	this->setWindowIcon(IconManager::getApplicationIcon());
	this->setMinimumSize(QSize(800, 600));

	// Connect Signals
	this->connect(m_list, &QListWidget::itemSelectionChanged, this, &CreateProjectDialog::slotShowInfo);
	this->connect(m_search, &LineEdit::textChanged, this, &CreateProjectDialog::slotSearch);
	this->connect(m_name, &LineEdit::textChanged, this, &CreateProjectDialog::slotCheckCreateEnabledState);
	this->connect(m_createButton, &PushButton::clicked, this, &CreateProjectDialog::slotCreate);
	this->connect(cancelButton, &PushButton::clicked, this, &CreateProjectDialog::closeCancel);
}

ot::CreateProjectDialog::~CreateProjectDialog() {

}

void ot::CreateProjectDialog::setProjectTemplates(const std::list<ProjectTemplateInformation>& _templates) {
	std::list<ProjectTemplateInformation> customTemplates;
	std::list<ProjectTemplateInformation> defaultTemplates;

	for (const ProjectTemplateInformation& info : _templates) {
		if (info.getIsDefault()) {
			defaultTemplates.push_back(info);
		}
		else {
			customTemplates.push_back(info);
		}
	}

	for (const ProjectTemplateInformation& info : customTemplates) {
		this->addListEntry(info);
	}

	if (!customTemplates.empty() && !defaultTemplates.empty()) {
		QListWidgetItem* separatorItem = new QListWidgetItem;
		separatorItem->setFlags(Qt::NoItemFlags);
		separatorItem->setSizeHint(QSize(20, 20));
		m_list->addItem(separatorItem);

		QFrame* separatorFrame = new QFrame;
		separatorFrame->setFrameShape(QFrame::HLine);
		m_list->setItemWidget(separatorItem, separatorFrame);
	}

	for (const ProjectTemplateInformation& info : defaultTemplates) {
		this->addListEntry(info);
	}
}

void ot::CreateProjectDialog::setCurrentProjectName(const QString& _name) {
	m_name->setText(_name);
}

QString ot::CreateProjectDialog::getProjectType(void) const {
	QList<QListWidgetItem*> items = m_list->selectedItems();
	if (items.count() == 1) {
		return items.front()->text();
	}
	else if (!items.isEmpty()) {
		OT_LOG_E("Invalid selection");
	}
	return QString();
}

QString ot::CreateProjectDialog::getProjectName(void) const {
	return m_name->text();
}

void ot::CreateProjectDialog::showEvent(QShowEvent* _event) {
	QMetaObject::invokeMethod(this, &CreateProjectDialog::slotFocusName, Qt::QueuedConnection);
	Dialog::showEvent(_event);
}

void ot::CreateProjectDialog::slotFocusName(void) {
	m_name->setFocus();
}

void ot::CreateProjectDialog::slotShowInfo(void) {
	QList<QListWidgetItem*> items = m_list->selectedItems();
	if (items.isEmpty()) {
		m_info->clear();
		this->setWindowTitle(OT_CREATEPROJECTDIALOG_TitlePrefix);
	}
	else if (items.count() == 1) {
		QString info = items.front()->data(Qt::UserRole).toString();
		if (info.isEmpty()) {
			info = items.front()->text();
		}
		m_info->setText(info);
		this->setWindowTitle(OT_CREATEPROJECTDIALOG_TitlePrefix " (" + items.front()->text() + ")");
	}
	else {
		OT_LOG_E("Invalid selection");
	}

	this->slotCheckCreateEnabledState();
}

void ot::CreateProjectDialog::slotSearch(void) {
	m_list->blockSignals(true);
	QString searchString = m_search->text();
	if (searchString.isEmpty()) {
		for (int i = 0; i < m_list->count(); i++) {
			m_list->item(i)->setHidden(false);
		}
	}
	else {
		for (int i = 0; i < m_list->count(); i++) {
			bool hidden = !(m_list->item(i)->text().contains(searchString, Qt::CaseInsensitive));
			m_list->item(i)->setHidden(hidden);
			if (hidden) {
				m_list->item(i)->setSelected(false);
			}
		}
	}
	m_list->blockSignals(false);
	this->slotShowInfo();
}

void ot::CreateProjectDialog::slotCreate(void) {
	QString newName = m_name->text();
	if (newName.isEmpty()) {
		QMessageBox msg(QMessageBox::Warning, "Invalid Data", "Please provide a valid project name.", QMessageBox::Ok, this);
		msg.exec();
		return;
	}

	QString templateName;
	QList<QListWidgetItem*> items = m_list->selectedItems();
	if (items.size() != 1) {
		QMessageBox msg(QMessageBox::Warning, "Invalid Data", "Please select a project template.", QMessageBox::Ok, this);
		msg.exec();
		return;
	}
	else {
		templateName = items.front()->text();
	}

	Q_EMIT createProject(newName, templateName);
}

void ot::CreateProjectDialog::slotCheckCreateEnabledState(void) {
	m_createButton->setEnabled(this->isDataValid());
}

bool ot::CreateProjectDialog::isDataValid(void) const {
	return (m_list->selectedItems().count() == 1) && !m_name->text().isEmpty();
}

void ot::CreateProjectDialog::clear(void) {
	m_list->clear();
	m_info->clear();
}

void ot::CreateProjectDialog::addListEntry(const ProjectTemplateInformation& _info) {
	QListWidgetItem* newItem = new QListWidgetItem;
	
	QFont font = newItem->font();
	font.setPixelSize(32);
	newItem->setFont(font);

	newItem->setText(QString::fromStdString(_info.getName()));
	newItem->setIcon(IconManager::getIcon(QString::fromStdString(_info.getIconSubPath())));
	newItem->setData(Qt::UserRole, QVariant(QString::fromStdString(_info.getDescription())));

	m_list->addItem(newItem);
}
