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
#include "OTWidgets/Label.h"
#include "OTWidgets/LineEdit.h"
#include "OTWidgets/CheckBox.h"
#include "OTWidgets/PushButton.h"
#include "OTGui/GraphicsItemCfg.h"
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
	ot::Label* itemNameLabel = new ot::Label("Item Name:");
	m_itemNameInput = new ot::LineEdit;
	m_itemNameInput->setToolTip("The name of the root item that will be exported.");

	ot::Label* fileNameLabel = new ot::Label("File Name:");
	m_fileInput = new ot::FilePathEdit(ot::FilePathEdit::SaveFileMode);
	m_fileInput->setBrowseTitle("Select GraphicsItem Export");
	m_fileInput->setFileFilter("GraphicsItem Files (*.ot.json)");
	m_fileInput->getLineEdit()->setToolTip("File export location.");

	ot::Label* autoAlignLabel = new ot::Label("Auto Align Item:");
	m_autoAlignInput = new ot::CheckBox;
	m_autoAlignInput->setToolTip("If enabled the item will be moved to (x: 0; y: 0) during export.");

	ot::Label* moveableItemLabel = new ot::Label("Item Moveable:");
	m_moveableItemInput = new ot::CheckBox;
	m_moveableItemInput->setToolTip("If enabled the exported item will be moveable.");

	ot::Label* itemGridSnapLabel = new ot::Label("Item Grid Snap:");
	m_itemGridSnapInput = new ot::CheckBox;
	m_itemGridSnapInput->setToolTip("If enabled the exported item will snap to the grid when grid snapping is enabled.");

	ot::PushButton* buttonExport = new ot::PushButton("Export");
	ot::PushButton* buttonCancel = new ot::PushButton("Cancel");

	// Initialize data
	m_autoAlignInput->setChecked(m_designer->getExportConfig().getExportConfigFlags() & GraphicsItemDesignerExportConfig::AutoAlign);
	m_moveableItemInput->setChecked(m_designer->getExportConfig().getExportConfigFlags() & GraphicsItemDesignerExportConfig::MoveableItem);
	m_itemGridSnapInput->setChecked(m_designer->getExportConfig().getExportConfigFlags() & GraphicsItemDesignerExportConfig::ItemGridSnap);

	// Initialize name and path
	QString newItemName = m_designer->getExportConfig().getItemName();
	if (newItemName.isEmpty()) newItemName = "New Item";
	m_itemNameInput->setText(newItemName);

	QString newItemPath = m_designer->getExportConfig().getFileName();
	newItemPath.replace('\\', '/');

	if (newItemPath.isEmpty()) {
		QByteArray envPath = qgetenv("OPENTWIN_DEV_ROOT");
		if (envPath.isEmpty()) {
			OT_LOG_W("OpenTwin dev root is not set in the environment");
			newItemPath = "C:/OpenTwin";
		}
		else {
			newItemPath = QString::fromStdString(envPath.toStdString());
		}

		newItemPath.replace('\\', '/');

		while (newItemPath.endsWith('/')) {
			newItemPath.removeLast();
		}

		newItemPath.append("/Assets/GraphicsItems/New Item.ot.json");
	}

	QStringList newItemPathList = newItemPath.split('/', Qt::SkipEmptyParts);
	newItemPathList.removeLast();

	newItemPath.clear();
	for (const QString& str : newItemPathList) {
		newItemPath.append(str + "/");
	}
	newItemPath.append(newItemName + ".ot.json");
	m_fileInput->setFilePath(newItemPath);

	// Setup layouts
	rootLayout->addLayout(inputLayout, 1);
	rootLayout->addLayout(buttonLayout);

	inputLayout->addWidget(itemNameLabel, 0, 0);
	inputLayout->addWidget(m_itemNameInput, 0, 1);
	inputLayout->addWidget(fileNameLabel, 1, 0);
	inputLayout->addWidget(m_fileInput->getQWidget(), 1, 1);
	inputLayout->addWidget(autoAlignLabel, 2, 0);
	inputLayout->addWidget(m_autoAlignInput, 2, 1);
	inputLayout->addWidget(moveableItemLabel, 3, 0);
	inputLayout->addWidget(m_moveableItemInput, 3, 1);
	inputLayout->addWidget(itemGridSnapLabel, 4, 0);
	inputLayout->addWidget(m_itemGridSnapInput, 4, 1);
	inputLayout->setColumnStretch(1, 1);

	buttonLayout->addStretch(1);
	buttonLayout->addWidget(buttonExport);
	buttonLayout->addWidget(buttonCancel);

	// Initialize window
	this->setWindowTitle("Export As GraphicsItem");
	this->setMinimumSize(500, 250);

	// Connect signals
	this->connect(m_itemNameInput, &ot::LineEdit::textChanged, this, &GraphicsItemDesignerExportDialog::slotUpdateFilePath);
	this->connect(buttonExport, &ot::PushButton::clicked, this, &GraphicsItemDesignerExportDialog::slotExport);
	this->connect(buttonCancel, &ot::PushButton::clicked, this, &GraphicsItemDesignerExportDialog::slotCancel);
}

GraphicsItemDesignerExportDialog::~GraphicsItemDesignerExportDialog() {}

GraphicsItemDesignerExportConfig GraphicsItemDesignerExportDialog::createExportConfig(void) const {
	GraphicsItemDesignerExportConfig newExportConfig;

	newExportConfig.setItemName(m_itemNameInput->text());
	newExportConfig.setFileName(m_fileInput->getFilePath());
	newExportConfig.setExportConfigFlag(GraphicsItemDesignerExportConfig::AutoAlign, m_autoAlignInput->isChecked());
	newExportConfig.setExportConfigFlag(GraphicsItemDesignerExportConfig::MoveableItem, m_moveableItemInput->isChecked());
	newExportConfig.setExportConfigFlag(GraphicsItemDesignerExportConfig::ItemGridSnap, m_itemGridSnapInput->isChecked());

	return newExportConfig;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private slots

void GraphicsItemDesignerExportDialog::slotExport(void) {
	// Check user inputs
	if (m_itemNameInput->text().isEmpty()) {
		OT_LOG_E("No item name set");
		return;
	}

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

	this->close(ot::Dialog::Ok);
}

void GraphicsItemDesignerExportDialog::slotCancel(void) {
	this->close(ot::Dialog::Cancel);
}

void GraphicsItemDesignerExportDialog::slotUpdateFilePath(void) {
	QString newPath = m_fileInput->getFilePath();

	if (!newPath.endsWith(".ot.json")) return;

	newPath.replace('\\', '/');
	QStringList newItemPathList = newPath.split('/', Qt::SkipEmptyParts);
	newItemPathList.removeLast();

	newPath.clear();
	for (const QString& str : newItemPathList) {
		newPath.append(str + "/");
	}

	if (m_itemNameInput->text().isEmpty()) {
		newPath.append("New Item");
	}
	else {
		newPath.append(m_itemNameInput->text());
	}
	newPath.append(".ot.json");

	m_fileInput->setFilePath(newPath);
}