//! @file GraphicsItemDesignerImageExportDialog.cpp
//! @author Alexander Kuester (alexk95)
//! @date June 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OToolkit header
#include "GraphicsItemDesigner.h"
#include "GraphicsItemDesignerView.h"
#include "GraphicsItemDesignerScene.h"
#include "GraphicsItemDesignerImageExportDialog.h"

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTWidgets/Label.h"
#include "OTWidgets/CheckBox.h"
#include "OTWidgets/LineEdit.h"
#include "OTWidgets/PushButton.h"
#include "OTWidgets/IconManager.h"
#include "OTWidgets/ImagePreview.h"
#include "OTWidgets/DoubleSpinBox.h"
#include "OTWidgets/PathBrowseEdit.h"

// Qt header
#include <QtCore/qfile.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qsplitter.h>
#include <QtWidgets/qmessagebox.h>

GraphicsItemDesignerImageExportDialog::GraphicsItemDesignerImageExportDialog(GraphicsItemDesigner* _designer) 
	: m_designer(_designer)
{
	OTAssertNullptr(m_designer);

	// Create layouts
	QVBoxLayout* rootLayout = new QVBoxLayout(this);
	QSplitter* mainSplitter = new QSplitter;
	QWidget* inputWidget = new QWidget;
	QGridLayout* inputLayout = new QGridLayout(inputWidget);
	QHBoxLayout* buttonLayout = new QHBoxLayout;

	// Create controls
	ot::Label* pathLabel = new ot::Label("File:");
	m_pathInput = new ot::PathBrowseEdit(ot::PathBrowseMode::WriteFile);
	m_pathInput->setBrowseTitle("Select Image Export");
	m_pathInput->setFileFilter("Image Files (*.png)");

	ot::Label* showGridLabel = new ot::Label("Show Grid:");
	m_showGrid = new ot::CheckBox;

	ot::Label* topMarginsLabel = new ot::Label("Margins Top:");
	m_topMarginsInput = new ot::DoubleSpinBox;

	ot::Label* leftMarginsLabel = new ot::Label("Margins Left:");
	m_leftMarginsInput = new ot::DoubleSpinBox;

	ot::Label* rightMarginsLabel = new ot::Label("Margins Right:");
	m_rightMarginsInput = new ot::DoubleSpinBox;

	ot::Label* bottomMarginsLabel = new ot::Label("Margins Bottom:");
	m_bottomMarginsInput = new ot::DoubleSpinBox;

	m_imagePreview = new ot::ImagePreview;
	
	ot::PushButton* buttonExport = new ot::PushButton("Export");
	ot::PushButton* buttonCancel = new ot::PushButton("Cancel");

	// Setup layouts
	rootLayout->addWidget(mainSplitter, 1);
	rootLayout->addLayout(buttonLayout);

	mainSplitter->addWidget(inputWidget);
	mainSplitter->addWidget(m_imagePreview);

	inputLayout->addWidget(pathLabel, 0, 0);
	inputLayout->addWidget(m_pathInput->getQWidget(), 0, 1);
	inputLayout->addWidget(showGridLabel, 1, 0);
	inputLayout->addWidget(m_showGrid, 1, 1);
	inputLayout->addWidget(topMarginsLabel, 2, 0);
	inputLayout->addWidget(m_topMarginsInput, 2, 1);
	inputLayout->addWidget(leftMarginsLabel, 3, 0);
	inputLayout->addWidget(m_leftMarginsInput, 3, 1);
	inputLayout->addWidget(rightMarginsLabel, 4, 0);
	inputLayout->addWidget(m_rightMarginsInput, 4, 1);
	inputLayout->addWidget(bottomMarginsLabel, 5, 0);
	inputLayout->addWidget(m_bottomMarginsInput, 5, 1);
	inputLayout->setColumnStretch(1, 1);

	buttonLayout->addStretch(1);
	buttonLayout->addWidget(buttonExport);
	buttonLayout->addWidget(buttonCancel);

	// Initialize data
	m_showGrid->setChecked(m_designer->getImageExportConfig().getShowGrid());
	QMarginsF oldMargins = m_designer->getImageExportConfig().getMargins();
	m_topMarginsInput->setValue(oldMargins.top());
	m_leftMarginsInput->setValue(oldMargins.left());
	m_rightMarginsInput->setValue(oldMargins.right());
	m_bottomMarginsInput->setValue(oldMargins.bottom());

	// Initialize file name
	m_pathInput->setPath(m_designer->getImageExportConfig().getFileName());

	// Initialize window
	this->setWindowTitle("Export As Image");
	this->setWindowIcon(ot::IconManager::getApplicationIcon());

	// Connect signals
	this->connect(buttonExport, &ot::PushButton::clicked, this, &GraphicsItemDesignerImageExportDialog::slotExport);
	this->connect(buttonCancel, &ot::PushButton::clicked, this, &GraphicsItemDesignerImageExportDialog::closeCancel);
	
	this->connect(m_showGrid, &ot::CheckBox::stateChanged, this, &GraphicsItemDesignerImageExportDialog::slotUpdatePreview);
	this->connect(m_topMarginsInput, &ot::DoubleSpinBox::valueChanged, this, &GraphicsItemDesignerImageExportDialog::slotUpdatePreview);
	this->connect(m_leftMarginsInput, &ot::DoubleSpinBox::valueChanged, this, &GraphicsItemDesignerImageExportDialog::slotUpdatePreview);
	this->connect(m_rightMarginsInput, &ot::DoubleSpinBox::valueChanged, this, &GraphicsItemDesignerImageExportDialog::slotUpdatePreview);
	this->connect(m_bottomMarginsInput, &ot::DoubleSpinBox::valueChanged, this, &GraphicsItemDesignerImageExportDialog::slotUpdatePreview);

	m_imagePreview->resize(50, 50);
	this->slotUpdatePreview();
}

GraphicsItemDesignerImageExportDialog::~GraphicsItemDesignerImageExportDialog() {

}

GraphicsItemDesignerImageExportConfig GraphicsItemDesignerImageExportDialog::createImageExportConfig(void) const {
	GraphicsItemDesignerImageExportConfig newConfig;
	
	newConfig.setFileName(m_pathInput->getPath());
	
	QMarginsF newMargins;
	newMargins.setTop(m_topMarginsInput->value());
	newMargins.setLeft(m_leftMarginsInput->value());
	newMargins.setRight(m_rightMarginsInput->value());
	newMargins.setBottom(m_bottomMarginsInput->value());
	newConfig.setMargins(newMargins);

	newConfig.setShowGrid(m_showGrid->isChecked());

	return newConfig;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private slots

void GraphicsItemDesignerImageExportDialog::slotExport(void) {
	const QString path = m_pathInput->getPath();

	if (path.isEmpty()) {
		OT_LOG_E("No image path set");
		return;
	}

	QFile file(path);

	if (file.exists()) {
		QMessageBox msgBox(
			QMessageBox::NoIcon, 
			"Item Exists", "The file \"" + path + "\" already exists. Do you want to replace it?",
			QMessageBox::Yes | QMessageBox::No, 
			this
		);

		if (msgBox.exec() != QMessageBox::Yes) {
			return;
		}
	}

	if (!m_imagePreview->getImage().save(path)) {
		OT_LOG_E("Failed to save file \"" + path.toStdString() + "\"");
		return;
	}

	OT_LOG_I("Image saved \"" + path.toStdString() + "\"");

	this->closeDialog(ot::Dialog::Ok);
}

void GraphicsItemDesignerImageExportDialog::slotUpdatePreview(void) {
	m_imagePreview->setImage(m_designer->getView()->getDesignerScene()->exportAsImage(this->createImageExportConfig()));
}