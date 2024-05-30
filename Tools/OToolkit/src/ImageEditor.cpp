//! @file ImageEditor.cpp
//! @author Alexander Kuester (alexk95)
//! @date April 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// Toolkit header
#include "ImageEditor.h"
#include "OToolkitAPI/OToolkitAPI.h"

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCore/Logger.h"
#include "OTCore/StringHelper.h"
#include "OTWidgets/Label.h"
#include "OTWidgets/SpinBox.h"
#include "OTWidgets/CheckBox.h"
#include "OTWidgets/Splitter.h"
#include "OTWidgets/ColorStyle.h"
#include "OTWidgets/PushButton.h"
#include "OTWidgets/ImagePreview.h"
#include "OTGui/ColorStyleTypes.h"
#include "OTWidgets/ColorPickButton.h"
#include "OTWidgets/BasicWidgetView.h"
#include "OTWidgets/GlobalColorStyle.h"

// Qt header
#include <QtCore/qfile.h>
#include <QtCore/qtimer.h>
#include <QtWidgets/qmenu.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qaction.h>
#include <QtWidgets/qshortcut.h>
#include <QtWidgets/qscrollarea.h>
#include <QtWidgets/qfiledialog.h>

ImageEditorToolBar::ImageEditorToolBar() {
	using namespace ot;
	Label* fromLabel = new Label("From:");
	m_fromColor = new ColorPickButton(QColor(0, 0, 0));
	m_fromColor->setMinimumWidth(100);
	m_fromColor->useAlpha(true);

	Label* toLabel = new Label("To:");
	m_toColor = new ColorPickButton(QColor(0, 0, 0));
	m_toColor->useAlpha(true);

	Label* toleranceLabel = new Label("Tolerance:");
	m_tolerance = new SpinBox;

	Label* useDiffLabel = new Label("Use Difference:");
	m_useDiff = new CheckBox;

	Label* checkAlphaLabel = new Label("Check Alpha:");
	m_checkAlpha = new CheckBox;

	Label* useOriginAlphaLabel = new Label("Use Original Alpha:");
	m_useOriginalAlpha = new CheckBox;

	m_spinDelayTimer = new QTimer;
	m_spinDelayTimer->setInterval(1000);
	m_spinDelayTimer->setSingleShot(true);

	this->addWidget(fromLabel);
	this->addWidget(m_fromColor);
	this->addWidget(toLabel);
	this->addWidget(m_toColor);
	this->addWidget(toleranceLabel);
	this->addWidget(m_tolerance);
	this->addWidget(useDiffLabel);
	this->addWidget(m_useDiff);
	this->addWidget(checkAlphaLabel);
	this->addWidget(m_checkAlpha);
	this->addWidget(useOriginAlphaLabel);
	this->addWidget(m_useOriginalAlpha);
	this->addSeparator();

	this->connect(m_fromColor, &ColorPickButton::colorChanged, this, &ImageEditorToolBar::slotDataChanged);
	this->connect(m_toColor, &ColorPickButton::colorChanged, this, &ImageEditorToolBar::slotDataChanged);
	this->connect(m_tolerance, &SpinBox::valueChanged, this, &ImageEditorToolBar::slotSpinChanged);
	this->connect(m_useDiff, &CheckBox::stateChanged, this, &ImageEditorToolBar::slotDataChanged);
	this->connect(m_checkAlpha, &CheckBox::stateChanged, this, &ImageEditorToolBar::slotDataChanged);
	this->connect(m_useOriginalAlpha, &CheckBox::stateChanged, this, &ImageEditorToolBar::slotDataChanged);
	this->connect(m_spinDelayTimer, &QTimer::timeout, this, &ImageEditorToolBar::slotDataChanged);
}

ImageEditorToolBar::~ImageEditorToolBar() {

}

void ImageEditorToolBar::setFromColor(const QColor& _color) {
	m_fromColor->setColor(_color);
	this->slotDataChanged();
}

QColor ImageEditorToolBar::fromColor(void) const {
	return m_fromColor->color();
}

QColor ImageEditorToolBar::toColor(void) const {
	return m_toColor->color();
}

int ImageEditorToolBar::tolerance(void) const {
	return m_tolerance->value();
}

bool ImageEditorToolBar::useDifference(void) const {
	return m_useDiff->isChecked();
}

bool ImageEditorToolBar::checkAlpha(void) const {
	return m_checkAlpha->isChecked();
}

bool ImageEditorToolBar::useOriginalAlpha(void) const {
	return m_useOriginalAlpha->isChecked();
}

void ImageEditorToolBar::slotDataChanged(void) {
	Q_EMIT dataChanged();
}

void ImageEditorToolBar::slotSpinChanged(void) {
	m_spinDelayTimer->stop();
	m_spinDelayTimer->start();
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ImageEditor::ImageEditor() {

}

ImageEditor::~ImageEditor() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// API base functions

bool ImageEditor::runTool(QMenu* _rootMenu, otoolkit::ToolWidgets& _content) {
	using namespace ot;
	
	// Create layouts
	Splitter* rootSplitter = new Splitter;
	m_root = this->createCentralWidgetView(rootSplitter, "Image Editor");
	_content.addView(m_root);

	QWidget* lLayW = new QWidget;
	QVBoxLayout* lLay = new QVBoxLayout(lLayW);

	QWidget* rLayW = new QWidget;
	QVBoxLayout* rLay = new QVBoxLayout(rLayW);

	// Create controls
	PushButton* btnImport = new PushButton("Import");
	PushButton* btnCalculate = new PushButton("Calculate");
	PushButton* btnExport = new PushButton("Export");

	m_original = new ImagePreview;
	m_converted = new ImagePreview;

	m_toolBar = new ImageEditorToolBar;

	QScrollArea* scrollL = new QScrollArea;
	scrollL->setWidgetResizable(true);
	scrollL->setWidget(m_original);

	QScrollArea* scrollR = new QScrollArea;
	scrollR->setWidgetResizable(true);
	scrollR->setWidget(m_converted);

	// Setup controls

	// Setup layouts
	rootSplitter->addWidget(lLayW);
	rootSplitter->addWidget(rLayW);

	lLay->addWidget(scrollL);
	lLay->addWidget(btnImport);
	lLay->addWidget(btnCalculate);

	rLay->addWidget(scrollR);
	rLay->addWidget(btnExport);

	// Create menu
	QAction* actionImport = _rootMenu->addAction("Import Image");
	QAction* actionCalculate = _rootMenu->addAction("Calculate");
	QAction* actionExport = _rootMenu->addAction("Export Image");
	
	// Initialize data
	QShortcut* shortcutOpen = new QShortcut(QKeySequence("Ctrl+O"), rootSplitter);
	QShortcut* shortcutSave = new QShortcut(QKeySequence("Ctrl+S"), rootSplitter);
	shortcutOpen->setContext(Qt::WidgetWithChildrenShortcut);
	shortcutSave->setContext(Qt::WidgetWithChildrenShortcut);
	
	// Connect signals
	this->connect(actionImport, &QAction::triggered, this, &ImageEditor::slotImport);
	this->connect(btnImport, &QPushButton::clicked, this, &ImageEditor::slotImport);
	this->connect(shortcutOpen, &QShortcut::activated, this, &ImageEditor::slotImport);

	this->connect(actionCalculate, &QAction::triggered, this, &ImageEditor::slotCalculate);
	this->connect(btnCalculate, &QPushButton::clicked, this, &ImageEditor::slotCalculate);
	this->connect(m_toolBar, &ImageEditorToolBar::dataChanged, this, &ImageEditor::slotCalculate);

	this->connect(actionExport, &QAction::triggered, this, &ImageEditor::slotExport);
	this->connect(btnExport, &QPushButton::clicked, this, &ImageEditor::slotExport);
	this->connect(shortcutSave, &QShortcut::activated, this, &ImageEditor::slotExport);

	this->connect(m_original, &ImagePreview::imagePixedClicked, this, &ImageEditor::slotOriginClicked);
	
	_content.setToolBar(m_toolBar);
	return true;
}

void ImageEditor::restoreToolSettings(QSettings& _settings) {

}

bool ImageEditor::prepareToolShutdown(QSettings& _settings) {
	return true;
}

// ###########################################################################################################################################################################################################################################################################################################################

void ImageEditor::slotOriginClicked(const QPoint& _px) {
	if (_px.x() >= 0 && _px.x() < m_original->image().width() && _px.y() >= 0 && _px.y() < m_original->image().height()) {
		m_toolBar->setFromColor(m_original->image().pixelColor(_px));
	}
	else {
		OT_LOG_E("PX issue");
	}
}

void ImageEditor::slotImport(void) {
	auto settings = otoolkit::api::getGlobalInterface()->createSettingsInstance();

	m_currentFileName = QFileDialog::getOpenFileName(m_root->getViewWidget(), "Open Image", settings->value("ImageEditor.LastImportFile", QString()).toString(), "Images (*.png *.jpg *.jpeg *.bmp *.gif *.tiff)");
	if (m_currentFileName.isEmpty()) return;

	settings->setValue("ImageEditor.LastImportFile", m_currentFileName);

	QImage img(m_currentFileName);
	m_original->setImage(img);
	m_original->setFocus();

	this->slotCalculate();
}

void ImageEditor::slotCalculate(void) {
	QImage result = m_original->image();

	int fR = m_toolBar->fromColor().red();
	int fG = m_toolBar->fromColor().green();
	int fB = m_toolBar->fromColor().blue();
	int fA = m_toolBar->fromColor().alpha();

	int fRf = fR - m_toolBar->tolerance();
	int fGf = fG - m_toolBar->tolerance();
	int fBf = fB - m_toolBar->tolerance();
	int fAf = fA - m_toolBar->tolerance();

	int fRt = fR + m_toolBar->tolerance();
	int fGt = fG + m_toolBar->tolerance();
	int fBt = fB + m_toolBar->tolerance();
	int fAt = fA + m_toolBar->tolerance();

	QColor tc = m_toolBar->toColor();
	int tR = tc.red();
	int tG = tc.green();
	int tB = tc.blue();
	int tA = tc.alpha();

	bool checkAlpha = m_toolBar->checkAlpha();
	bool useDiff = m_toolBar->useDifference();
	bool useOriginAlpha = m_toolBar->useOriginalAlpha();

	for (int y = 0; y < result.height(); y++) {
		for (int x = 0; x < result.width(); x++) {
			QColor c = result.pixelColor(x, y);
			int r = c.red();
			int g = c.green();
			int b = c.blue();
			int a = c.alpha();

			if (r >= fRf && r <= fRt &&
				g >= fGf && g <= fGt &&
				b >= fBf && b <= fBt) 
			{
				if (checkAlpha && (a < fAf || a > fAt)) {
				}
				else {
					if (useDiff) {
						int rr = tR + (r - fR);
						int rg = tG + (g - fG);
						int rb = tB + (b - fB);
						if (rr < 0) rr = 0;
						if (rg < 0) rg = 0;
						if (rb < 0) rb = 0;
						if (rr > 255) rr = 0;
						if (rg > 255) rg = 0;
						if (rb > 255) rb = 0;
						if (useOriginAlpha) {
							result.setPixelColor(x, y, QColor(rr, rg, rb, a));
						}
						else {
							int ra = tA + (a - fA);
							result.setPixelColor(x, y, QColor(rr, rg, rb, ra));
						}
					}
					else {
						if (useOriginAlpha) {
							tc.setAlpha(a);
						}
						result.setPixelColor(x, y, tc);
					}

				}
			}
		}
	}

	m_converted->setImage(result);
}

void ImageEditor::slotExport(void) {
	if (m_currentFileName.isEmpty() || m_converted->image().isNull()) {
		OT_LOG_W("Noting to export");
		return;
	}

	QString fileName = QFileDialog::getSaveFileName(m_root->getViewWidget(), "Save Image", m_currentFileName, "Images (*.png *.jpg *.jpeg *.bmp *.gif *.tiff)");
	if (fileName.isEmpty()) return;

	m_converted->image().save(fileName);
}
