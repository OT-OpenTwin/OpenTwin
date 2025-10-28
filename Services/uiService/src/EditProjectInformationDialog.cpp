// @otlicense

// OpenTwin header
#include "AppBase.h"
#include "ProjectOverviewPreviewBox.h"
#include "EditProjectInformationDialog.h"
#include "ModelState.h"
#include "UserManagement.h"
#include "ProjectManagement.h"
#include "OTCore/String.h"
#include "OTCore/ReturnMessage.h"
#include "OTGui/FileExtension.h"
#include "OTWidgets/Label.h"
#include "OTWidgets/ComboBox.h"
#include "OTWidgets/TextEditor.h"
#include "OTWidgets/ToolButton.h"
#include "OTWidgets/PushButton.h"
#include "OTWidgets/IconManager.h"
#include "OTWidgets/PlainTextEdit.h"
#include "OTWidgets/BasicValidator.h"
#include "OTWidgets/ImagePainterWidget.h"
#include "OTWidgets/ImagePainterManager.h"
#include "OTCommunication/ActionTypes.h"

// Qt header
#include <QtCore/qfile.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qfiledialog.h>

EditProjectInformationDialog::EditProjectInformationDialog(const std::string& _callbackUrl, const std::string& _callbackAction, const LoginData& _logInData, const std::string& _projectName, QWidget* _parent)
	: Dialog(_parent), m_callbackUrl(_callbackUrl), m_callbackAction(_callbackAction), c_toolButtonSize(24, 24), m_logInData(_logInData)
{
	m_projectInformation.setProjectName(_projectName);

	using namespace ot;

	// Create widgets
	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	
	// Title section
	QHBoxLayout* titleLayout = new QHBoxLayout;
	titleLayout->addWidget(new Label(QString::fromStdString("Project: " + m_projectInformation.getProjectName())));
	mainLayout->addLayout(titleLayout);

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
	m_category->setValidator(new BasicValidator(BasicValidator::NameRanges, BasicValidator::NameRanges, BasicValidator::NameRanges));
	dataLayout->addWidget(m_category, r++, 1);

	dataLayout->addWidget(new Label("Tags:", this), r, 0);
	m_tags = new PlainTextEdit(this);
	m_tags->setToolTip("Project Tags (separated by blanks, tabulators or line breaks)");
	m_tags->setReadOnly(false);
	m_tags->setFixedHeight(100);
	m_tags->setValidator(new BasicValidator(BasicValidator::NameRanges, BasicValidator::AllRanges, BasicValidator::AllRanges));
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
	updateInformationEntry();

	using namespace ot;

	ProjectManagement projectManager(m_logInData);
	projectManager.updateAdditionalInformation(m_projectInformation);

	JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, JsonString(m_callbackAction, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_Config, JsonObject(m_projectInformation, doc.GetAllocator()), doc.GetAllocator());

	std::string responseStr;
	AppBase::instance()->sendExecute(m_callbackUrl, doc.toJson(), responseStr);
	ot::ReturnMessage response = ot::ReturnMessage::fromJson(responseStr);
	if (!response.isOk()) {
		AppBase::instance()->showErrorPrompt("Edit Project Information", "Could not update project information.", response.getWhat());
		return;
	}

	closeOk();
}

void EditProjectInformationDialog::slotChanged() {
	
}

void EditProjectInformationDialog::slotUploadImage() {
	using namespace ot;
	std::string filter = FileExtension::toFilterString({ FileExtension::Png, FileExtension::Jpeg, FileExtension::Svg });
	QString file = QFileDialog::getOpenFileName(this, "Select Image File", "", QString::fromStdString(filter));
	if (file.isEmpty()) {
		return;
	}

	// Determine file 
	std::string fileStd = file.toStdString();
	size_t ix = fileStd.rfind('.');
	if (ix == std::string::npos) {
		AppBase::instance()->showErrorPrompt("Upload Image", "Could not upload image.", "The selected file does not have a valid file extension.");
		return;
	}
	std::string fileExtension = fileStd.substr(ix + 1);
	FileExtension::DefaultFileExtension ext = FileExtension::stringToFileExtension(fileExtension);
	if (ext == FileExtension::Unknown) {
		AppBase::instance()->showErrorPrompt("Upload Image", "Could not upload image.", "The selected file has an unsupported file extension (*.\"" + fileExtension + "\").");
		return;
	}

	bool isImage = false;
	ImageFileFormat format = ImageFileFormat::PNG;
	FileExtension::toImageFileFormat(ext, isImage);
	if (!isImage) {
		AppBase::instance()->showErrorPrompt("Upload Image", "Could not upload image.", "The selected file is not a supported image file (*.\"" + fileExtension + "\").");
		return;
	}

	// Read file data
	QFile qfile(file);
	if (!qfile.open(QIODevice::ReadOnly)) {
		AppBase::instance()->showErrorPrompt("Upload Image", "Could not upload image.", "The selected file could not be opened for reading.");
		return;
	}

	QByteArray fileData = qfile.readAll();
	qfile.close();

	std::vector<char> imageData(fileData.size());
	std::memcpy(imageData.data(), fileData.data(), fileData.size());

	ImagePainter* painter = ImagePainterManager::createFromRawData(imageData, format);
	m_imageWidget->setPainter(painter);
	
	m_projectInformation.setImageData(std::move(imageData), format);
}

void EditProjectInformationDialog::slotTakeScreenshot() {

}

void EditProjectInformationDialog::slotRemoveImage() {
	m_imageWidget->setPainter(nullptr);
	m_projectInformation.setImageData(std::vector<char>(), ot::ImageFileFormat::PNG);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Helper

void EditProjectInformationDialog::initializeData() {
	using namespace ot;

	// Fetch project information
	ProjectManagement projectManager(m_logInData);
	std::list<std::string> projectNamesList = { m_projectInformation.getProjectName() };
	if (!projectManager.readProjectsInfo(projectNamesList) || projectNamesList.empty()) {
		OT_LOG_E("Failed to read project information for project \"" + m_projectInformation.getProjectName() + "\"");
		return;
	}

	ot::ProjectInformation projInfo = projectManager.getProjectInformation(m_projectInformation.getProjectName());
	if (projInfo.getCollectionName().empty()) {
		OT_LOG_E("Failed to read project information for project \"" + m_projectInformation.getProjectName() + "\"");
		return;
	}
	m_projectInformation = ot::ExtendedProjectInformation(projInfo);

	// Fetch Description
	std::string description;
	ot::DocumentSyntax syntax = ot::DocumentSyntax::PlainText;
	if (ModelState::readProjectDescription(m_projectInformation.getCollectionName(), description, syntax)) {
		m_projectInformation.setDescription(description);
		m_projectInformation.setDescriptionSyntax(syntax);
	}

	// Fetch Image
	std::vector<char> imageData;
	ot::ImageFileFormat imageFormat = ot::ImageFileFormat::PNG;
	if (ModelState::readProjectPreviewImage(m_projectInformation.getCollectionName(), imageData, imageFormat)) {
		m_projectInformation.setImageData(std::move(imageData), imageFormat);
	}

	// Update widgets
	m_descriptionSyntax->clear();
	m_descriptionSyntax->addItem(QString::fromStdString(toString(DocumentSyntax::PlainText)));
	m_descriptionSyntax->addItem(QString::fromStdString(toString(DocumentSyntax::Markdown)));
	m_descriptionSyntax->addItem(QString::fromStdString(toString(DocumentSyntax::HTML)));
	m_descriptionSyntax->setCurrentText(QString::fromStdString(toString(m_projectInformation.getDescriptionSyntax())));
	
	m_description->setPlainText(QString::fromStdString(m_projectInformation.getDescription()));

	if (!m_projectInformation.getImageData().empty()) {
		ImagePainter* painter = ImagePainterManager::createFromRawData(m_projectInformation.getImageData(), m_projectInformation.getImageFormat());
		m_imageWidget->setPainter(painter);
	}

	m_tags->clear();
	for (const std::string& tag : m_projectInformation.getTags()) {
		m_tags->appendPlainText(QString::fromStdString(tag));
	}

	m_category->clear();
	QStringList categories;
	std::list<ot::ProjectInformation> projects;
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

void EditProjectInformationDialog::updateInformationEntry() {
	m_projectInformation.setCategory(ot::String::removePrefixSuffix(m_category->currentText().toStdString(), " \t\n\r"));
	
	std::list<std::string> tags;
	QString tagsText = m_tags->toPlainText();
	tagsText.replace('\n', ' ').replace('\t', ' ');
	QStringList tagList = tagsText.split(' ', Qt::SkipEmptyParts);
	for (const QString& tag : tagList) {
		tags.push_back(tag.toStdString());
	}
	m_projectInformation.setTags(tags);

	m_projectInformation.setDescription(m_description->toPlainText().toStdString());
	m_projectInformation.setDescriptionSyntax(ot::stringToDocumentSyntax(m_descriptionSyntax->currentText().toStdString()));
}
