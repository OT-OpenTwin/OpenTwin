// @otlicense
// File: FMNewProjectDialog.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTFMC/FMDirectory.h"
#include "OTFMC/FMNewProjectDialog.h"
#include "OTWidgets/Label.h"
#include "OTWidgets/ComboBox.h"
#include "OTWidgets/LineEdit.h"
#include "OTWidgets/ToolButton.h"
#include "OTWidgets/PushButton.h"
#include "OTWidgets/IconManager.h"
#include "OTWidgets/PlainTextEdit.h"
#include "OTWidgets/MessageDialog.h"
#include "OTWidgets/PathBrowseEdit.h"
#include "OTWidgets/ToolTipHandler.h"

// Qt header
#include <QtCore/qdir.h>
#include <QtWidgets/qlayout.h>

ot::FMNewProjectDialog::FMNewProjectDialog(QWidget* _parent) 
	: Dialog(_parent), m_replaceIgnoreFile(false),
	m_directory(nullptr), m_ignoreText(nullptr), m_defaultIgnorePatterns(nullptr)
{
	setDialogFlags(DialogCfg::IdealFit | DialogCfg::RecenterOnF11);

	QVBoxLayout* mainLayout = new QVBoxLayout(this);

	QHBoxLayout* dirLayout = new QHBoxLayout;
	mainLayout->addLayout(dirLayout);
	
	dirLayout->addWidget(new Label("Root Directory:", this));
	m_directory = new PathBrowseEdit(PathBrowseMode::Directory, this);
	dirLayout->addWidget(m_directory->getQWidget(), 1);

	QHBoxLayout* ignoreTitleLayout = new QHBoxLayout;
	mainLayout->addLayout(ignoreTitleLayout);

	ignoreTitleLayout->addWidget(new Label("Ignore Patterns", this));
	ignoreTitleLayout->addStretch(1);
	ignoreTitleLayout->addWidget(new Label("Preset:", this));
	m_defaultIgnorePatterns = new ComboBox(this);
	m_defaultIgnorePatterns->addItem("", QVariant(static_cast<uint64_t>(DefaultIgnorePattern::Unknown)));
	m_defaultIgnorePatterns->addItem("Programming", QVariant(static_cast<uint64_t>(DefaultIgnorePattern::Programming)));
	ignoreTitleLayout->addWidget(m_defaultIgnorePatterns);
	connect(m_defaultIgnorePatterns, &ComboBox::currentIndexChanged, this, &FMNewProjectDialog::slotApplyIgnorePatternPreset);

	QHBoxLayout* ingoreInputLayout = new QHBoxLayout;
	mainLayout->addLayout(ingoreInputLayout);
	m_ignoreText = new PlainTextEdit(this);
	ingoreInputLayout->addWidget(m_ignoreText, 1);

	QHBoxLayout* buttonLayout = new QHBoxLayout;
	mainLayout->addLayout(buttonLayout);

	ToolButton* helpButton = new ToolButton(IconManager::getIcon("Button/Help.png"), "", this);
	helpButton->setFixedSize(32, 32);
	helpButton->setToolTip("Show Help");
	buttonLayout->addWidget(helpButton);
	buttonLayout->addStretch(1);

	PushButton* confirmButton = new PushButton("Initialize Project", this);
	buttonLayout->addWidget(confirmButton);
	confirmButton->setDefault(true);

	PushButton* cancelButton = new PushButton("Cancel", this);
	buttonLayout->addWidget(cancelButton);
	connect(helpButton, &ToolButton::clicked, this, &FMNewProjectDialog::slotShowHelp);
	connect(confirmButton, &PushButton::clicked, this, &FMNewProjectDialog::slotConfirm);
	connect(cancelButton, &PushButton::clicked, this, &FMNewProjectDialog::closeCancel);

	// Setup dialog
	setWindowTitle("Initialize New Project");
	setMinimumSize(600, 400);
	resize(800, 600);
}

ot::FMNewProjectDialog::~FMNewProjectDialog() {
}

ot::FMNewProjectInfo ot::FMNewProjectDialog::getNewProjectInfo() const {
	FMNewProjectInfo info;
	info.setRootDirectory(m_directory->getPath().toStdString());
	info.setReplaceIgnoreFile(m_replaceIgnoreFile);

	FMIgnoreFile ignoreFile;
	ignoreFile.parseFromText(m_ignoreText->toPlainText().toStdString());
	info.setIgnoreFile(std::move(ignoreFile));

	return info;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Slots

void ot::FMNewProjectDialog::slotConfirm() {
	// Reset values
	m_replaceIgnoreFile = false;

	// Ensure directory provided
	QString pth = m_directory->getPath();
	if (pth.isEmpty()) {
		QPoint pos = m_directory->getLineEdit()->mapToGlobal(m_directory->getLineEdit()->rect().bottomLeft());
		ToolTipHandler::showToolTip(pos, "Please select a directory to initialize the project at");
		return;
	}

	// Ensure directory exists
	QDir dir(pth);
	if (!dir.exists()) {
		QPoint pos = m_directory->getLineEdit()->mapToGlobal(m_directory->getLineEdit()->rect().bottomLeft());
		ToolTipHandler::showToolTip(pos, "The specified directory does not exist");
		return;
	}
	
	// Check ignore patterns
	FMIgnoreFile ignoreFile;
	if (!ignoreFile.parseFromText(m_ignoreText->toPlainText().toStdString())) {
		MessageDialogCfg cfg;
		cfg.setButtons(MessageDialogCfg::Yes | MessageDialogCfg::Cancel);
		cfg.setIcon(MessageDialogCfg::Warning);
		cfg.setTitle("Invalid Ignore Patterns");
		cfg.setText("Some ignore patterns could not be parsed correctly. "
			"Do you want to proceed anyway?");
		if (MessageDialog::showDialog(cfg, this) != MessageDialogCfg::Yes) {
			return;
		}
	}

	// Check for existing project
	FMDirectory root = FMDirectory::fromFileSystem(pth.toStdString(), FMIgnoreFile(), FMDirectory::ScanFlag::ScanFiles);
	auto cacheFileInfo = root.getFile(OpenTwinCacheFileName);
	if (cacheFileInfo.has_value()) {
		MessageDialogCfg cfg;
		cfg.setButtons(MessageDialogCfg::Ok);
		cfg.setIcon(MessageDialogCfg::Warning);
		cfg.setTitle("Existing Project Detected");
		cfg.setText("An existing OpenTwin project was detected in the selected directory. "
			"Please select a different directory to initialize a new project or delete the \"" + std::string(OpenTwinCacheFileName) + "\".");
		
		MessageDialog::showDialog(cfg, this);
		return;
	}

	// Check for existing ignore file
	auto ignoreFileInfo = root.getFile(OpenTwinIgnoreFileName);
	if (ignoreFileInfo.has_value()) {
		if (!ignoreFile.hasPatterns()) {
			// No ignore patterns provided, no need to replace existing ignore file
			m_replaceIgnoreFile = false;
		}
		else {
			MessageDialogCfg cfg;
			cfg.setButtons(MessageDialogCfg::Yes | MessageDialogCfg::No | MessageDialogCfg::Cancel);
			cfg.setIcon(MessageDialogCfg::Question);
			cfg.setTitle("Existing Ignore File");
			cfg.setText("An existing " + std::string(OpenTwinIgnoreFileName) + " file was found in the selected directory. "
				"Do you want to replace the existing ignore file?");
			
			auto result = MessageDialog::showDialog(cfg, this);
			if (result == MessageDialogCfg::Yes) {
				m_replaceIgnoreFile = true;
			}
			else if (result != MessageDialogCfg::No) {
				return;
			}
		}
	}

	closeOk();
}

void ot::FMNewProjectDialog::slotShowHelp() {

}

void ot::FMNewProjectDialog::slotApplyIgnorePatternPreset() {
	DefaultIgnorePattern pattern = static_cast<DefaultIgnorePattern>(m_defaultIgnorePatterns->currentData().toULongLong());

	switch (pattern) {
	case DefaultIgnorePattern::Unknown:
		break;

	case DefaultIgnorePattern::Programming:
		applyProgrammingIgnorePattern();
		break;

	default:
		OT_LOG_E("Unknown ignore pattern preset selected: \"" + m_defaultIgnorePatterns->currentText().toStdString());
		break;
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Helper

void ot::FMNewProjectDialog::applyProgrammingIgnorePattern() {
	m_ignoreText->setPlainText(
		"# =============================================================\n"
		"# Common Programming Project Ignore Patterns\n"
		"# Covers: C, C++, CMake, Java, Python, JavaScript, HTML, and IDEs\n"
		"# =============================================================\n"
		"\n"
		"# === IDE / Editor directories ===\n"
		".vs/\n"
		".vscode/\n"
		".idea/\n"
		".settings/\n"
		".classpath\n"
		".project\n"
		"*.sublime-workspace\n"
		"*.sublime-project\n"
		"*.swp\n"
		"*.bak\n"
		"\n"
		"# === Build output ===\n"
		"bin/\n"
		"obj/\n"
		"build/\n"
		"dist/\n"
		"out/\n"
		"Debug/\n"
		"Release/\n"
		"x64/\n"
		"x86/\n"
		"CMakeFiles/\n"
		"CMakeCache.txt\n"
		"Makefile\n"
		"*.make\n"
		"*.ninja\n"
		"*.manifest\n"
		"\n"
		"# === Compiled binaries ===\n"
		"*.exe\n"
		"*.dll\n"
		"*.so\n"
		"*.dylib\n"
		"*.lib\n"
		"*.a\n"
		"*.pdb\n"
		"*.obj\n"
		"*.o\n"
		"*.out\n"
		"\n"
		"# === Java ===\n"
		"*.class\n"
		"*.jar\n"
		"*.war\n"
		"*.ear\n"
		"\n"
		"# === Python ===\n"
		"__pycache__/\n"
		"*.pyc\n"
		"*.pyo\n"
		"*.pyd\n"
		".pytest_cache/\n"
		".tox/\n"
		".mypy_cache/\n"
		"env/\n"
		"venv/\n"
		"\n"
		"# === Node.js / JavaScript ===\n"
		"node_modules/\n"
		"npm-debug.log*\n"
		"yarn-debug.log*\n"
		"yarn-error.log*\n"
		"package-lock.json\n"
		".next/\n"
		".nuxt/\n"
		"coverage/\n"
		"*.tsbuildinfo\n"
		"\n"
		"# === Web / HTML / CSS ===\n"
		".sass-cache/\n"
		".cache/\n"
		"*.log\n"
		"*.tmp\n"
		"*.temp\n"
		"*.map\n"
		"\n"
		"# === Misc / Tools ===\n"
		"*.iml\n"
		"*.user\n"
		"*.suo\n"
		"*.idb\n"
		"*.tlog\n"
		"*.ilk\n"
		"*.pch\n"
		"*.ipch\n"
		"*.db\n"
		"*.opendb\n"
		"*.VC.db\n"
		"*.VC.opendb\n"
		".clangd/\n"
		".cache/\n"
		".ccls-cache/\n"
		"\n"
		"# === Logs & Reports ===\n"
		"*.log\n"
		"*.trace\n"
		"*.dmp\n"
		"*.stackdump\n"
		"\n"
		"# === System / OS files ===\n"
		".DS_Store\n"
		"Thumbs.db\n"
		"desktop.ini\n"
		"\n"
		"# === OpenTwin Log Files ===\n"
		".otlog\n"
		".otlog.json\n"
		".otlogbuf\n"
		"\n"
	);
}
