// @otlicense

// OpenTwin header
#include "ProjectOverviewPreviewBox.h"
#include "EditProjectInformationDialog.h"
#include "ModelState.h"
#include "UserManagement.h"
#include "ProjectManagement.h"
#include "OTWidgets/Label.h"
#include "OTWidgets/ComboBox.h"
#include "OTWidgets/TextEditor.h"
#include "OTWidgets/ToolButton.h"
#include "OTWidgets/PushButton.h"
#include "OTWidgets/IconManager.h"
#include "OTWidgets/PlainTextEdit.h"
#include "OTWidgets/ImagePainterWidget.h"
#include "OTWidgets/ImagePainterManager.h"

// Qt header
#include <QtWidgets/qlayout.h>

EditProjectInformationDialog::EditProjectInformationDialog(const LoginData& _logInData, const ot::ProjectInformation& _project, QWidget* _parent)
	: Dialog(_parent), c_toolButtonSize(24, 24), m_logInData(_logInData), m_projectInformation(_project)
{
	using namespace ot;

	// Create widgets
	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	
	// Title section
	QHBoxLayout* titleLayout = new QHBoxLayout;
	titleLayout->addWidget(new Label(QString::fromStdString("Project: " + m_projectInformation.getProjectName())));

	// Image section
	QHBoxLayout* imageLayout = new QHBoxLayout;
	mainLayout->addLayout(imageLayout);
	m_imageWidget = new ImagePainterWidget(this);
	m_imageWidget->setFixedSize(ProjectOverviewPreviewBox::previewImageSize());

	

	QVBoxLayout* imageButtonLayout = new QVBoxLayout;
	imageButtonLayout->addStretch(1);

	QVBoxLayout* spacerLayout = new QVBoxLayout;
	QWidget* spacerWidget = new QWidget(this);
	spacerWidget->setFixedSize(c_toolButtonSize);
	spacerLayout->addWidget(spacerWidget);

	imageLayout->addStretch(1);
	imageLayout->addLayout(imageButtonLayout);
	imageLayout->addWidget(m_imageWidget);
	imageLayout->addLayout(spacerLayout);
	imageLayout->addStretch(1);
	
	ToolButton* uploadImageButton = new ToolButton(IconManager::getIcon("Button/Add.png"), "", this);
	uploadImageButton->setToolTip("Upload Image");
	uploadImageButton->setFixedSize(c_toolButtonSize);
	imageButtonLayout->addWidget(uploadImageButton);

	ToolButton* takeScreenshotButton = new ToolButton(IconManager::getIcon("Button/Camera.png"), "", this);
	takeScreenshotButton->setToolTip("Take Screenshot");
	takeScreenshotButton->setFixedSize(c_toolButtonSize);
	imageButtonLayout->addWidget(takeScreenshotButton);

	ToolButton* removeImageButton = new ToolButton(IconManager::getIcon("Button/Remove.png"), "", this);
	removeImageButton->setToolTip("Remove Image");
	removeImageButton->setFixedSize(c_toolButtonSize);
	imageButtonLayout->addWidget(removeImageButton);
	imageButtonLayout->addStretch(1);

	// Data section
	QGridLayout* dataLayout = new QGridLayout;
	mainLayout->addLayout(dataLayout);
	dataLayout->setColumnStretch(1, 1);
	int r = 0;
	dataLayout->addWidget(new Label("Category:", this), r, 0);
	m_category = new ComboBox(this);
	m_category->setEditable(true);
	m_category->setToolTip("Project Category");
	dataLayout->addWidget(m_category, r++, 1);

	dataLayout->addWidget(new Label("Tags:", this), r, 0);
	m_tags = new PlainTextEdit(this);
	m_tags->setToolTip("Project Tags (separated by blanks, tabulators or line breaks)");
	m_tags->setReadOnly(false);
	m_tags->setFixedHeight(100);
	dataLayout->addWidget(m_tags, r++, 1);

	// Description
	QVBoxLayout* descriptionLayout = new QVBoxLayout;
	mainLayout->addLayout(descriptionLayout, 1);

	QHBoxLayout* descriptionHeaderLayout = new QHBoxLayout;
	descriptionHeaderLayout->addStretch();
	descriptionHeaderLayout->setContentsMargins(0, 0, 0, 0);
	descriptionLayout->addLayout(descriptionHeaderLayout);

	descriptionHeaderLayout->addWidget(new Label("Description:", this));
	m_descriptionSyntax = new ComboBox(this);
	m_descriptionSyntax->setToolTip("Description Syntax");
	m_descriptionSyntax->setEditable(false);
	descriptionHeaderLayout->addWidget(m_descriptionSyntax);
	descriptionHeaderLayout->addStretch(1);

	m_description = new TextEditor(this);
	m_description->setPlaceholderText("Project Description");
	descriptionLayout->addWidget(m_description, 1);

	// Create buttons
	QHBoxLayout* buttonLayout = new QHBoxLayout;
	mainLayout->addLayout(buttonLayout);
	buttonLayout->addStretch();
	
	m_confirmButton = new PushButton("Confirm", this);
	buttonLayout->addWidget(m_confirmButton);

	PushButton* cancelButton = new PushButton("Cancel", this);
	buttonLayout->addWidget(cancelButton);

	initializeData();
	
	resize(400, 800);
	setMinimumSize(300, 500);
	setWindowTitle("Edit Project Information");

	// Connect signals
	connect(uploadImageButton, &ToolButton::clicked, this, &EditProjectInformationDialog::slotUploadImage);
	connect(takeScreenshotButton, &ToolButton::clicked, this, &EditProjectInformationDialog::slotTakeScreenshot);
	connect(removeImageButton, &ToolButton::clicked, this, &EditProjectInformationDialog::slotRemoveImage);

	connect(m_confirmButton, &PushButton::clicked, this, &EditProjectInformationDialog::slotConfirm);
	connect(cancelButton, &PushButton::clicked, this, &EditProjectInformationDialog::closeCancel);

}

// ###########################################################################################################################################################################################################################################################################################################################

// Protected: Events

void EditProjectInformationDialog::showEvent(QShowEvent* _event) {
	if (m_firstShow) {
		m_firstShow = false;
		QTimer::singleShot(0, this, [this]() {
			m_description->setFocus();
			});
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Slots

void EditProjectInformationDialog::slotConfirm() {

}

void EditProjectInformationDialog::slotChanged() {
	
}

void EditProjectInformationDialog::slotUploadImage() {

}

void EditProjectInformationDialog::slotTakeScreenshot() {

}

void EditProjectInformationDialog::slotRemoveImage() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Helper

void EditProjectInformationDialog::initializeData() {
	using namespace ot;

	m_descriptionSyntax->clear();
	m_descriptionSyntax->addItem(QString::fromStdString(toString(DocumentSyntax::PlainText)));
	m_descriptionSyntax->addItem(QString::fromStdString(toString(DocumentSyntax::Markdown)));
	m_descriptionSyntax->addItem(QString::fromStdString(toString(DocumentSyntax::HTML)));

	std::vector<char> imageData;
	ImageFileFormat imageFormat = ImageFileFormat::PNG;
	if (ModelState::readProjectPreviewImage(m_projectInformation.getCollectionName(), imageData, imageFormat)) {
		ImagePainter* painter = ImagePainterManager::createFromRawData(imageData, imageFormat);
		m_imageWidget->setPainter(painter);
	}

	m_tags->clear();
	for (const std::string& tag : m_projectInformation.getTags()) {
		m_tags->appendPlainText(QString::fromStdString(tag));
	}

	m_category->clear();
	QStringList categories;
	std::list<ot::ProjectInformation> projects;
	ProjectManagement projectManager(m_logInData);
	bool exceeded = false;
	if (projectManager.findProjects("", 1000, projects, exceeded)) {
		for (const ot::ProjectInformation& proj : projects) {
			const std::string& category = proj.getCategory();
			if (!category.empty() && !categories.contains(QString::fromStdString(category))) {
				categories.append(QString::fromStdString(category));
			}
		}
	}
	m_category->addItems(categories);
	m_category->setCurrentText(QString::fromStdString(m_projectInformation.getCategory()));
}
