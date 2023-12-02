//! @file FAR.cpp
//! @author Alexander Kuester (alexk95)
//! @date November 2023
// ###########################################################################################################################################################################################################################################################################################################################

// Toolkit header
#include "FAR.h"
#include "OToolkitAPI/OToolkitAPI.h"

// Qt header
#include <QtCore/qdir.h>
#include <QtCore/qfile.h>
#include <QtCore/qtextstream.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qsplitter.h>
#include <QtWidgets/qlineedit.h>
#include <QtWidgets/qtabwidget.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qfiledialog.h>
#include <QtWidgets/qplaintextedit.h>

// std header
#include <thread>

#define FAR_LOG(___message) OTOOLKIT_LOG("FAR", ___message)
#define FAR_LOGW(___message) OTOOLKIT_LOGW("FAR", ___message)
#define FAR_LOGE(___message) OTOOLKIT_LOGE("FAR", ___message)

#define FAR_SEARCHMODE_FindText "Find Text"

bool FARFilter::validate(const FARFile& _file) const {
	// Whitelist
	if (!m_whitelistFiles.isEmpty()) {
		bool wok = false;
		for (const auto& e : m_whitelistFiles) {
			if (_file.fullPath().contains(e, Qt::CaseInsensitive)) {
				wok = true;
				break;
			}
		}
		if (!wok) return false;
	}

	// Blacklist
	for (const auto& e : m_blacklistFiles) {
		if (_file.fullPath().contains(e, Qt::CaseInsensitive)) {
			return false;
		}
	}
	return true;
}

bool FARFilter::validate(const FARDirectory& _dir) const {
	// Whitelist
	if (!m_whitelistDirectories.isEmpty()) {
		bool wok = false;
		for (const auto& e : m_whitelistDirectories) {
			if (_dir.fullPath().contains(e, Qt::CaseInsensitive)) {
				wok = true;
				break;
			}
		}
		if (!wok) return false;
	}

	// Blacklist
	for (const auto& e : m_blacklistDirectories) {
		if (_dir.fullPath().contains(e, Qt::CaseInsensitive)) {
			return false;
		}
	}
	return true;
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

FARFile::FARFile(const QString& _fullPath) {
	m_fullPath = _fullPath;
	m_fullPath.replace('\\', '/');
	m_nameOnly = m_fullPath.split('/').last();
	m_fileExtension = m_nameOnly.split('.').last();
}

FARFile::FARFile(const FARFile& _other) : m_fileExtension(_other.m_fileExtension), m_fullPath(_other.m_fullPath), m_nameOnly(_other.m_nameOnly) {}

bool FARFile::readAll(void) {
	QFile file(m_fullPath);
	if (file.exists()) {
		if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
			FAR_LOGE("Failed to open file for reading: \""  + m_fullPath + "\"");
			return false;
		}

		m_textLines.clear();

		QTextStream stream(&file);

		while (!stream.atEnd()) {
			m_textLines.append(stream.readLine());
		}

		file.close();
		return true;
	}
	else {
		FAR_LOGE("File does not exist \"" + m_fullPath + "\"");
		return false;
	}

}

bool FARFile::exists(void) const {
	QFile file(m_fullPath);
	return file.exists();
}

bool FARFile::findText(const QString& _text, std::list<FARMatch>& _matches, int& _longestText, int& _longestPath) const {
	int lineCt = 0;
	for (const QString& line : m_textLines) {
		if (line.contains(_text, Qt::CaseInsensitive)) {
			_longestText = std::max(_longestText, line.length());
			_longestPath = std::max(_longestPath, this->fullPath().length());
			_matches.push_back(FARMatch(this->fullPath(), lineCt, line));
		}
		lineCt++;
	}
	return true;
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

FARDirectory::FARDirectory(const QString& _fullPath) {
	m_fullPath = _fullPath;
	m_fullPath.replace('\\', '/');
	m_nameOnly = m_fullPath.split('/').last();
}

FARDirectory::FARDirectory(const FARDirectory& _parentDirectory, const QString& _directoryName) {
	m_fullPath = _parentDirectory.fullPath() + '/' + _directoryName;
	m_nameOnly = _directoryName;
}

FARDirectory::FARDirectory(const FARDirectory& _other) : m_fullPath(_other.m_fullPath), m_nameOnly(_other.m_nameOnly), m_files(_other.m_files), m_directories(_other.m_directories) {

}

bool FARDirectory::exists(void) const {
	QDir dir(m_fullPath);
	return dir.exists();
}

bool FARDirectory::scanFiles(const FARFilter& _filter, int& _counter) {
	m_files.clear();

	QDir directory(m_fullPath);
	if (!directory.exists()) {
		FAR_LOGE("Directory does not exist: \"" + m_fullPath + "\"");
		return false;
	}
	else {
		QStringList files = directory.entryList(QDir::Files | QDir::NoDotAndDotDot);
		for (QString f : files) {
			FARFile newFile(this->fullPath() + '/' + f);
			_counter++;

			if (_filter.validate(newFile)) {
				m_files.push_back(newFile);
			}
		}
		return true;
	}
}

bool FARDirectory::readAllFiles(bool _topLevelOnly) {
	for (FARFile& f : m_files) {
		if (!f.readAll()) return false;
	}
	if (!_topLevelOnly) {
		for (FARDirectory& d : m_directories) {
			if (!d.readAllFiles(false)) return false;
		}
	}
	return true;
}

bool FARDirectory::scanDirectories(const FARFilter& _filter, int& _counter, bool _topLevelOnly) {
	m_directories.clear();

	QDir directory(m_fullPath);
	if (!directory.exists()) {
		FAR_LOGE("Directory does not exist: \"" + m_fullPath + "\"");
		return false;
	}
	else {
		QStringList subdirectories = directory.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
		for (QString d : subdirectories) {
			FARDirectory newDir(*this, d);
			_counter++;

			if (_filter.validate(newDir)) {
				if (!_topLevelOnly) {
					if (!newDir.scanDirectories(_filter, _counter, true)) {
						return false;
					}
				}
				m_directories.push_back(newDir);
			}
		}
		return true;
	}
}

bool FARDirectory::scanAll(const FARFilter& _filter, int& _fileCounter, int& _directoryCounter, bool _topLevelOnly, bool _readFiles) {
	m_directories.clear();

	QDir directory(m_fullPath);
	if (!directory.exists()) {
		FAR_LOGE("Directory does not exist: \"" + m_fullPath + "\"");
		return false;
	}
	else {
		if (!this->scanFiles(_filter, _fileCounter)) return false;

		if (_readFiles) {
			if (!this->readAllFiles(true)) return false;
		}

		if (!this->scanDirectories(_filter, _directoryCounter, _topLevelOnly)) return false;

		if (!_topLevelOnly) {
			for (FARDirectory& d : m_directories) {
				if (!d.scanAll(_filter, _fileCounter, _directoryCounter, _topLevelOnly, _readFiles)) return false;
			}
		}

		return true;
	}
}

bool FARDirectory::findText(const QString& _text, std::list<FARMatch>& _matches, int& _longestText, int& _longestPath, bool _topLevelOnly) const {
	for (const FARFile& f : m_files) {
		if (!f.findText(_text, _matches, _longestText, _longestPath)) return false;
	}

	if (!_topLevelOnly) {
		for (const FARDirectory& d : m_directories) {
			if (!d.findText(_text, _matches, _longestText, _longestPath, false)) return false;
		}
	}

	return true;
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

FAR::FAR() {

}

FAR::~FAR() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// API base functions

QString FAR::toolName(void) const {
	return "FAR";
}

QWidget* FAR::runTool(QMenu* _rootMenu, std::list<QWidget*>& _statusWidgets, QSettings& _settings) {
	m_centralSplitter = new QSplitter;

	// Filter

	m_leftGridW = new QWidget;
	m_leftGrid = new QGridLayout(m_leftGridW);

	m_leftTitle = new QLabel("Filter");
	QFont f = m_leftTitle->font();
	f.setPointSize(12);
	f.setBold(true);
	m_leftTitle->setFont(f);

	m_whitelistFilesL = new QLabel("Whitelist Files:");
	m_whitelistFiles = new QPlainTextEdit;
	m_blacklistFilesL = new QLabel("Blacklist Files:");
	m_blacklistFiles = new QPlainTextEdit;
	m_whitelistDirectoriesL = new QLabel("Whitelist Directories:");
	m_whitelistDirectories = new QPlainTextEdit;
	m_blacklistDirectoriesL = new QLabel("Blacklist Directories:");
	m_blacklistDirectories = new QPlainTextEdit;

	m_leftGrid->addWidget(m_leftTitle, 0, 0);
	m_leftGrid->addWidget(m_whitelistFilesL, 1, 0);
	m_leftGrid->addWidget(m_whitelistFiles, 1, 1);
	m_leftGrid->addWidget(m_blacklistFilesL, 2, 0);
	m_leftGrid->addWidget(m_blacklistFiles, 2, 1);
	m_leftGrid->addWidget(m_whitelistDirectoriesL, 3, 0);
	m_leftGrid->addWidget(m_whitelistDirectories, 3, 1);
	m_leftGrid->addWidget(m_blacklistDirectoriesL, 4, 0);
	m_leftGrid->addWidget(m_blacklistDirectories, 4, 1);
	m_leftGrid->setColumnStretch(1, 1);

	m_centralSplitter->addWidget(m_leftGridW);

	// Find mode
	m_rightLayoutW = new QWidget;
	m_rightLayout = new QVBoxLayout(m_rightLayoutW);
	
	m_rightTopLayout = new QHBoxLayout;
	m_findModeTab = new QTabWidget;
	m_findModeTab->setTabPosition(QTabWidget::North);

	m_rootDirL = new QLabel("Root directory:");
	m_rootDir = new QLineEdit;
	m_browseRootDir = new QPushButton("Browse");

	m_rightTopLayout->addWidget(m_rootDirL);
	m_rightTopLayout->addWidget(m_rootDir, 1);
	m_rightTopLayout->addWidget(m_browseRootDir);

	m_rightLayout->addLayout(m_rightTopLayout);
	m_rightLayout->addWidget(m_findModeTab, 1);

	m_centralSplitter->addWidget(m_rightLayoutW);

	this->connect(m_browseRootDir, &QPushButton::clicked, this, &FAR::slotBrowseRoot);

	// Find mode: Plain text
	m_findTextLayoutW = new QWidget;
	m_findTextLayout = new QVBoxLayout(m_findTextLayoutW);
	m_findTextTopLayout = new QHBoxLayout;
	m_findTextButtonLayout = new QHBoxLayout;
	m_findTextL = new QLabel("Line contains:");
	m_findText = new QLineEdit;
	m_findTextBtn = new QPushButton(FAR_SEARCHMODE_FindText);

	m_findTextLayout->addLayout(m_findTextTopLayout);
	m_findTextLayout->addStretch(1);
	m_findTextLayout->addLayout(m_findTextButtonLayout);

	m_findTextTopLayout->addWidget(m_findTextL);
	m_findTextTopLayout->addWidget(m_findText, 1);

	m_findTextButtonLayout->addStretch(1);
	m_findTextButtonLayout->addWidget(m_findTextBtn);

	this->connect(m_findTextBtn, &QPushButton::clicked, this, &FAR::slotFindText);
	m_findModeTab->addTab(m_findTextLayoutW, FAR_SEARCHMODE_FindText);

	// Restore settings
	m_rootDir->setText(_settings.value("FAR.RootDir", QString()).toString());
	m_blacklistDirectories->setPlainText(_settings.value("FAR.BLD", QString()).toString());
	m_whitelistDirectories->setPlainText(_settings.value("FAR.WLD", QString()).toString());
	m_blacklistFiles->setPlainText(_settings.value("FAR.BLF", QString()).toString());
	m_whitelistFiles->setPlainText(_settings.value("FAR.WLF", QString()).toString());
	m_findText->setText(_settings.value("FAR.FindTxt", QString()).toString());

	return m_centralSplitter;
}

bool FAR::prepareToolShutdown(QSettings& _settings) {
	_settings.setValue("FAR.RootDir", m_rootDir->text());
	_settings.setValue("FAR.BLD", m_blacklistDirectories->toPlainText());
	_settings.setValue("FAR.WLD", m_whitelistDirectories->toPlainText());
	_settings.setValue("FAR.BLF", m_blacklistFiles->toPlainText());
	_settings.setValue("FAR.WLF", m_whitelistFiles->toPlainText());
	_settings.setValue("FAR.FindTxt", m_findText->text());

	return true;
}

void FAR::slotBrowseRoot(void) {
	m_rootDir->setText(QFileDialog::getExistingDirectory(m_centralSplitter, "Select root browse directory", m_rootDir->text(), QFileDialog::ShowDirsOnly));
}

void FAR::slotFindText(void) {
	FARFilter filter;
	this->setupFilter(filter);

	this->slotLock();

	std::thread t(&FAR::workerFindText, this, m_rootDir->text(), m_findText->text(), filter);
	t.detach();
}

void FAR::slotLock(void) {
	m_centralSplitter->setEnabled(false);
}

void FAR::slotUnlock(void) {
	m_centralSplitter->setEnabled(true);
}

void FAR::setupFilter(FARFilter& _filter) const {
	_filter.setWhitelistFiles(m_whitelistFiles->toPlainText().split("\n", QString::SkipEmptyParts));
	_filter.setBlacklistFiles(m_blacklistFiles->toPlainText().split("\n", QString::SkipEmptyParts));
	_filter.setWhitelistDirectories(m_whitelistDirectories->toPlainText().split("\n", QString::SkipEmptyParts));
	_filter.setBlacklistDirectories(m_blacklistDirectories->toPlainText().split("\n", QString::SkipEmptyParts));
}

void FAR::workerFindText(QString _root, QString _text, FARFilter _filter) {
	this->workerFindTextLogic(_root, _text, _filter);
	QMetaObject::invokeMethod(this, &FAR::slotUnlock, Qt::QueuedConnection);
}

void FAR::workerFindTextLogic(const QString& _root, const QString& _text, const FARFilter& _filter) {
	FAR_LOG("> Starting \"" FAR_SEARCHMODE_FindText "\"");

	FARDirectory root(_root);
	if (!root.exists()) {
		FAR_LOGE("Invalid root directory \"" + root.fullPath() + "\"");
		return;
	}

	if (_text.isEmpty()) {
		FAR_LOGE("No text to find set");
		return;
	}

	FAR_LOG("> Scanning root directory content (this may take a while)...");
	int fileCt = 0;
	int dirCt = 0;
	if (!root.scanAll(_filter, fileCt, dirCt, false, true)) {
		FAR_LOGE("> Search cancelled.");
		return;
	}

	FAR_LOG("> Scan completed with " + QString::number(fileCt) + " files and " + QString::number(dirCt) + " directories");
	FAR_LOG("> Searching for matches (this may take a while)...");

	int longestPath = 0;
	int longestText = 0;
	std::list<FARMatch> matches;

	// Find matches
	if (!root.findText(_text, matches, longestText, longestPath, false)) {
		FAR_LOGE("> Search cancelled.");
		return;
	}

	if (matches.empty()) {
		FAR_LOG("> \"" FAR_SEARCHMODE_FindText "\" ended without any matches");
		return;
	}


	QString pathSuffix(" @ line ");
	longestPath = longestPath + pathSuffix.length() + 11;

	QString msg = "> \"" FAR_SEARCHMODE_FindText "\" ended with " + QString::number(matches.size()) + " matches:\n\n";
	for (FARMatch& match : matches) {
		QString path = match.fullPath() + pathSuffix + QString::number(match.line()) + ':';
		while (path.length() < longestPath) path.append(' ');
		QString txt = match.text();
		while (txt.startsWith(' ') || txt.startsWith('\t')) txt.remove(0, 1);
		msg.append(path + txt + "\n");
	}

	FAR_LOG(msg);
}