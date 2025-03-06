//! @file GraphicsItemDesignerExportDialog.cpp
//! @author Alexander Kuester (alexk95)
//! @date June 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OToolkit header
#include "GraphicsItemDesigner.h"
#include "GraphicsItemDesignerNavigation.h"
#include "GraphicsItemDesignerExportDialog.h"

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/GraphicsItemCfg.h"
#include "OTWidgets/Label.h"
#include "OTWidgets/LineEdit.h"
#include "OTWidgets/CheckBox.h"
#include "OTWidgets/PushButton.h"
#include "OTWidgets/IconManager.h"
#include "OTWidgets/FilePathEdit.h"
#include "OTWidgets/PropertyInputFilePath.h"

// Qt header
#include <QtCore/qfile.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qmessagebox.h>

GraphicsItemDesignerExportDialog::GraphicsItemDesignerExportDialog(GraphicsItemDesigner* _designer)
	: m_designer(_designer)
{
	OTAssertNullptr(m_designer);

	// Create layouts
	QVBoxLayout* rootLayout = new QVBoxLayout(this);
	QGridLayout* inputLayout = new QGridLayout;
	QHBoxLayout* buttonLayout = new QHBoxLayout;

	// Create controls
	ot::Label* fileNameLabel = new ot::Label("File Name:");
	m_fileInput = new ot::FilePathEdit(ot::FilePathEdit::SaveFileMode);
	m_fileInput->setBrowseTitle("Select GraphicsItem Export");
	m_fileInput->setFileFilter("GraphicsItem Files (*.ot.json)");
	m_fileInput->getLineEdit()->setToolTip("File export location.");

	ot::Label* autoAlignLabel = new ot::Label("Auto Align Item:");
	m_autoAlignInput = new ot::CheckBox;
	m_autoAlignInput->setToolTip("If enabled the item will be moved to (x: 0; y: 0) during export.\nThis should be enabled.");

	ot::PushButton* buttonExport = new ot::PushButton("Export");
	ot::PushButton* buttonCancel = new ot::PushButton("Cancel");

	// Initialize data
	m_autoAlignInput->setChecked(m_designer->getExportConfig().getExportConfigFlags() & GraphicsItemDesignerExportConfig::AutoAlign);

	// Initialize name and path
	m_fileInput->setFilePath(m_designer->getExportConfig().getFileName());
	
	// Setup layouts
	rootLayout->addLayout(inputLayout, 1);
	rootLayout->addLayout(buttonLayout);

	inputLayout->addWidget(fileNameLabel, 0, 0);
	inputLayout->addWidget(m_fileInput->getQWidget(), 0, 1);
	inputLayout->addWidget(autoAlignLabel, 1, 0);
	inputLayout->addWidget(m_autoAlignInput, 1, 1);
	inputLayout->setColumnStretch(1, 1);

	buttonLayout->addStretch(1);
	buttonLayout->addWidget(buttonExport);
	buttonLayout->addWidget(buttonCancel);

	// Initialize window
	this->setWindowTitle("Export As GraphicsItem");
	this->setWindowIcon(ot::IconManager::getApplicationIcon());
	this->setMinimumSize(500, 100);

	// Connect signals
	this->connect(buttonExport, &ot::PushButton::clicked, this, &GraphicsItemDesignerExportDialog::slotExport);
	this->connect(buttonCancel, &ot::PushButton::clicked, this, &GraphicsItemDesignerExportDialog::slotCancel);
}

GraphicsItemDesignerExportDialog::~GraphicsItemDesignerExportDialog() {}

GraphicsItemDesignerExportConfig GraphicsItemDesignerExportDialog::createExportConfig(void) const {
	GraphicsItemDesignerExportConfig newExportConfig;

	newExportConfig.setFileName(m_fileInput->getFilePath());
	newExportConfig.setExportConfigFlag(GraphicsItemDesignerExportConfig::AutoAlign, m_autoAlignInput->isChecked());

	return newExportConfig;
}

void GraphicsItemDesignerExportDialog::showEvent(QShowEvent* _event) {
	ot::Dialog::showEvent(_event);
	m_fileInput->getLineEdit()->setFocus();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private slots

void GraphicsItemDesignerExportDialog::slotExport(void) {
	// Check user inputs
	if (m_fileInput->getFilePath().isEmpty()) {
		OT_LOG_E("No file path set");
		return;
	}

	// Create export config
	GraphicsItemDesignerExportConfig exportConfig = this->createExportConfig();
	
	// Generate config
	ot::GraphicsItemCfg* config = m_designer->getNavigation()->generateConfig(exportConfig);
	if (!config) {
		return;
	}

	// Export config
	ot::JsonDocument configDoc;
	config->addToJsonObject(configDoc, configDoc.GetAllocator());
	delete config;
	config = nullptr;

	QFile file(exportConfig.getFileName());

	if (file.exists()) {
		QMessageBox msgBox(QMessageBox::NoIcon, "Item Exists", "The file \"" + exportConfig.getFileName() + "\" already exists. Do you want to replace it?", QMessageBox::Yes | QMessageBox::No, this);
		if (msgBox.exec() != QMessageBox::Yes) return;
	}

	if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		OT_LOG_E("Failed to open file for writing \"" + m_fileInput->getFilePath().toStdString() + "\"");
		return;
	}

	file.write(QByteArray::fromStdString(configDoc.toJson()));
	file.close();

	OT_LOG_D("Graphics Item exported \"" + m_fileInput->getFilePath().toStdString() + "\"");

	this->closeDialog(ot::Dialog::Ok);
}

void GraphicsItemDesignerExportDialog::slotCancel(void) {
	this->closeDialog(ot::Dialog::Cancel);
}
