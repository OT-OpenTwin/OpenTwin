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
#include <QtCore/qdatetime.h>
#include <QtCore/qtextstream.h>
#include <QtCore/qregularexpression.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qgroupbox.h>
#include <QtWidgets/qcheckbox.h>
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

FARFilter::FARFilter() 
	: m_blacklistDirectoriesFlags(FARFilter::NoFlags), m_blacklistFilesFlags(FARFilter::NoFlags), m_whitelistDirectoriesFlags(FARFilter::NoFlags), m_whitelistFilesFlags(FARFilter::NoFlags)
{}

FARFilter::FARFilter(const FARFilter& _other)
	: m_whitelistDirectories(_other.m_whitelistDirectories),
	m_whitelistDirectoriesFlags(_other.m_whitelistDirectoriesFlags),
	m_blacklistDirectories(_other.m_blacklistDirectories),
	m_blacklistDirectoriesFlags(_other.m_blacklistDirectoriesFlags),
	m_blacklistFiles(_other.m_blacklistFiles),
	m_blacklistFilesFlags(_other.m_blacklistFilesFlags),
	m_whitelistFiles(_other.m_whitelistFiles),
	m_whitelistFilesFlags(_other.m_whitelistFilesFlags)
{};

bool FARFilter::validate(const FARFile& _file) const {
	// Whitelist
	if (!m_whitelistFiles.isEmpty()) {
		bool wok = false;
		for (const auto& e : m_whitelistFiles) {
			if (m_whitelistFilesFlags & FARFilter::StartsWith) {
				if (_file.fullPath().startsWith(e, ((m_whitelistFilesFlags & FARFilter::CheckCase) ? Qt::CaseSensitive : Qt::CaseInsensitive))) {
					wok = true;
					break;
				}
			}
			if (m_whitelistFilesFlags & FARFilter::Contains) {
				if (_file.fullPath().contains(e, ((m_whitelistFilesFlags & FARFilter::CheckCase) ? Qt::CaseSensitive : Qt::CaseInsensitive))) {
					wok = true;
					break;
				}
			}
			if (m_whitelistFilesFlags & FARFilter::EndsWith) {
				if (_file.fullPath().endsWith(e, ((m_whitelistFilesFlags & FARFilter::CheckCase) ? Qt::CaseSensitive : Qt::CaseInsensitive))) {
					wok = true;
					break;
				}
			}
			if (m_whitelistFilesFlags & FARFilter::AsRegex) {
				QRegularExpression regexp(e);
				if (regexp.match(_file.fullPath()).hasMatch()) {
					wok = true;
					break;
				}
			}
		}
		if (!wok) return false;
	}

	// Blacklist
	for (const auto& e : m_blacklistFiles) {
		if (m_blacklistFilesFlags & FARFilter::StartsWith) {
			if (_file.fullPath().startsWith(e, ((m_blacklistFilesFlags & FARFilter::CheckCase) ? Qt::CaseSensitive : Qt::CaseInsensitive))) {
				return false;
			}
		}
		if (m_blacklistFilesFlags & FARFilter::Contains) {
			if (_file.fullPath().contains(e, ((m_blacklistFilesFlags & FARFilter::CheckCase) ? Qt::CaseSensitive : Qt::CaseInsensitive))) {
				return false;
			}
		}
		if (m_blacklistFilesFlags & FARFilter::EndsWith) {
			if (_file.fullPath().endsWith(e, ((m_blacklistFilesFlags & FARFilter::CheckCase) ? Qt::CaseSensitive : Qt::CaseInsensitive))) {
				return false;
			}
		}
		if (m_blacklistFilesFlags & FARFilter::AsRegex) {
			QRegularExpression regexp(e);
			if (regexp.match(_file.fullPath()).hasMatch()) {
				return false;

			}
		}
	}
	return true;
}

bool FARFilter::validate(const FARDirectory& _dir) const {
	// Whitelist
	if (!m_whitelistDirectories.isEmpty()) {
		bool wok = false;
		for (const auto& e : m_whitelistDirectories) {
			if (m_whitelistDirectoriesFlags & FARFilter::StartsWith) {
				if (_dir.fullPath().startsWith(e, ((m_whitelistDirectoriesFlags & FARFilter::CheckCase) ? Qt::CaseSensitive : Qt::CaseInsensitive))) {
					wok = true;
					break;
				}
			}
			if (m_whitelistDirectoriesFlags & FARFilter::Contains) {
				if (_dir.fullPath().contains(e, ((m_whitelistDirectoriesFlags & FARFilter::CheckCase) ? Qt::CaseSensitive : Qt::CaseInsensitive))) {
					wok = true;
					break;
				}
			}
			if (m_whitelistDirectoriesFlags & FARFilter::EndsWith) {
				if (_dir.fullPath().endsWith(e, ((m_whitelistDirectoriesFlags & FARFilter::CheckCase) ? Qt::CaseSensitive : Qt::CaseInsensitive))) {
					wok = true;
					break;
				}
			}
			if (m_whitelistDirectoriesFlags & FARFilter::AsRegex) {
				QRegularExpression regexp(e);
				if (regexp.match(_dir.fullPath()).hasMatch()) {
					wok = true;
					break;
				}
			}
		}
		if (!wok) return false;
	}

	// Blacklist
	for (const auto& e : m_blacklistDirectories) {
		if (m_blacklistDirectoriesFlags & FARFilter::StartsWith) {
			if (_dir.fullPath().startsWith(e, ((m_blacklistDirectoriesFlags & FARFilter::CheckCase) ? Qt::CaseSensitive : Qt::CaseInsensitive))) {
				return false;
			}
		}
		if (m_blacklistDirectoriesFlags & FARFilter::Contains) {
			if (_dir.fullPath().contains(e, ((m_blacklistDirectoriesFlags & FARFilter::CheckCase) ? Qt::CaseSensitive : Qt::CaseInsensitive))) {
				return false;
			}
		}
		if (m_blacklistDirectoriesFlags & FARFilter::EndsWith) {
			if (_dir.fullPath().endsWith(e, ((m_blacklistDirectoriesFlags & FARFilter::CheckCase) ? Qt::CaseSensitive : Qt::CaseInsensitive))) {
				return false;
			}
		}
		if (m_blacklistDirectoriesFlags & FARFilter::AsRegex) {
			QRegularExpression regexp(e);
			if (regexp.match(_dir.fullPath()).hasMatch()) {
				return false;
			}
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

bool FARFile::findText(const QString& _text, FARFilter::FilterFlag _textFilter, std::list<FARMatch>& _matches, int& _longestText, int& _longestPath) const {
	int lineCt = 1;
	bool added = false;
	for (const QString& line : m_textLines) {
		added = false;
		if (_textFilter & FARFilter::StartsWith) {
			if (!added && line.startsWith(_text, ((_textFilter & FARFilter::CheckCase) ? Qt::CaseSensitive : Qt::CaseInsensitive))) {
				_longestText = std::max((qsizetype)_longestText, line.length());
				_longestPath = std::max((qsizetype)_longestPath, this->fullPath().length());
				_matches.push_back(FARMatch(this->fullPath(), lineCt, line));
				added = true;
			}
		}
		if (_textFilter & FARFilter::Contains) {
			if (!added && line.contains(_text, ((_textFilter & FARFilter::CheckCase) ? Qt::CaseSensitive : Qt::CaseInsensitive))) {
				_longestText = std::max((qsizetype)_longestText, line.length());
				_longestPath = std::max((qsizetype)_longestPath, this->fullPath().length());
				_matches.push_back(FARMatch(this->fullPath(), lineCt, line));
				added = true;
			}
		}
		if (_textFilter & FARFilter::EndsWith) {
			if (!added && line.endsWith(_text, ((_textFilter & FARFilter::CheckCase) ? Qt::CaseSensitive : Qt::CaseInsensitive))) {
				_longestText = std::max((qsizetype)_longestText, line.length());
				_longestPath = std::max((qsizetype)_longestPath, this->fullPath().length());
				_matches.push_back(FARMatch(this->fullPath(), lineCt, line));
				added = true;
			}
		}
		if (_textFilter & FARFilter::AsRegex) {
			QRegularExpression regexp(_text);
			if (!added && regexp.match(line).hasMatch()) {
				_longestText = std::max((qsizetype)_longestText, line.length());
				_longestPath = std::max((qsizetype)_longestPath, this->fullPath().length());
				_matches.push_back(FARMatch(this->fullPath(), lineCt, line));
				added = true;
			}
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

bool FARDirectory::findText(const QString& _text, FARFilter::FilterFlag _textFilter, std::list<FARMatch>& _matches, int& _longestText, int& _longestPath, bool _topLevelOnly) const {
	for (const FARFile& f : m_files) {
		if (!f.findText(_text, _textFilter, _matches, _longestText, _longestPath)) return false;
	}

	if (!_topLevelOnly) {
		for (const FARDirectory& d : m_directories) {
			if (!d.findText(_text, _textFilter, _matches, _longestText, _longestPath, false)) return false;
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

	m_whitelistFiles = this->createFilterGroup("Whitelist Files:", "FAR.WLF", _settings, m_leftGrid, 1);
	m_blacklistFiles = this->createFilterGroup("Blacklist Files:", "FAR.BLF", _settings, m_leftGrid, 2);
	m_whitelistDirectories = this->createFilterGroup("Whitelist Directories:", "FAR.WLD", _settings, m_leftGrid, 3);
	m_blacklistDirectories = this->createFilterGroup("Blacklist Directories:", "FAR.BLD", _settings, m_leftGrid, 4);

	m_whitelistFiles.label->setToolTip("The full path of a file must contain at least one of the entries (entries separated by newline). Disabled if no entries are provided.");
	m_whitelistFiles.label->setToolTipDuration(5000);
	m_whitelistFiles.edit->setToolTip("The full path of a file must contain at least one of the entries (entries separated by newline). Disabled if no entries are provided.");
	m_whitelistFiles.edit->setToolTipDuration(5000);

	m_blacklistFiles.label->setToolTip("The full path of a file may not contain any of the entries (entries separated by newline). Disabled if no entries are provided.");
	m_blacklistFiles.label->setToolTipDuration(5000);
	m_blacklistFiles.edit->setToolTip("The full path of a file may not contain any of the entries (entries separated by newline). Disabled if no entries are provided.");
	m_blacklistFiles.edit->setToolTipDuration(5000);

	m_whitelistDirectories.label->setToolTip("The full path of a directory must contain at least one of the entries (entries separated by newline). Note that nested directories may not be found if their parent directory does not match the filter. Disabled if no entries are provided.");
	m_whitelistDirectories.label->setToolTipDuration(5000);
	m_whitelistDirectories.edit->setToolTip("The full path of a directory must contain at least one of the entries (entries separated by newline). Note that nested directories may not be found if their parent directory does not match the filter. Disabled if no entries are provided.");
	m_whitelistDirectories.edit->setToolTipDuration(5000);

	m_blacklistDirectories.label->setToolTip("The full path of a directory may not contain any of the entries (entries separated by newline). Note that nested directories may not be found if their parent directory does not match the filter. Disabled if no entries are provided.");
	m_blacklistDirectories.label->setToolTipDuration(5000);
	m_blacklistDirectories.edit->setToolTip("The full path of a directory may not contain any of the entries (entries separated by newline). Note that nested directories may not be found if their parent directory does not match the filter. Disabled if no entries are provided.");
	m_blacklistDirectories.edit->setToolTipDuration(5000);

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
	m_findTextTopLayout = new QGridLayout;
	m_findTextButtonLayout = new QHBoxLayout;
	m_findTextL = new QLabel("Text:");
	m_findText = new QLineEdit;
	m_findTextMode = new QLabel("Mode:");
	m_findTextCaseSensitive = new QCheckBox("Case Sesitive");
	m_findTextStartsWith = new QCheckBox("Starts With");
	m_findTextContains = new QCheckBox("Contains");
	m_findTextEndsWith = new QCheckBox("Ends With");
	m_findTextRegex = new QCheckBox("Regex");
	m_findTextBtn = new QPushButton(FAR_SEARCHMODE_FindText);

	m_findTextLayout->addLayout(m_findTextTopLayout);
	m_findTextLayout->addStretch(1);
	m_findTextLayout->addLayout(m_findTextButtonLayout);

	m_findTextTopLayout->addWidget(m_findTextL, 0, 0);
	m_findTextTopLayout->addWidget(m_findText, 0, 1);
	m_findTextTopLayout->addWidget(m_findTextMode, 1, 0);
	m_findTextTopLayout->addWidget(m_findTextCaseSensitive, 1, 1);
	m_findTextTopLayout->addWidget(m_findTextStartsWith, 2, 1);
	m_findTextTopLayout->addWidget(m_findTextContains, 3, 1);
	m_findTextTopLayout->addWidget(m_findTextEndsWith, 4, 1);
	m_findTextTopLayout->addWidget(m_findTextRegex, 5, 1);

	m_findTextButtonLayout->addStretch(1);
	m_findTextButtonLayout->addWidget(m_findTextBtn);

	this->connect(m_findTextBtn, &QPushButton::clicked, this, &FAR::slotFindText);
	m_findModeTab->addTab(m_findTextLayoutW, FAR_SEARCHMODE_FindText);

	// Restore settings
	m_rootDir->setText(_settings.value("FAR.RootDir", QString()).toString());
	m_findText->setText(_settings.value("FAR.FindTxt", QString()).toString());
	m_findTextCaseSensitive->setChecked(_settings.value("FAR.FindTxt.C", false).toBool());
	m_findTextStartsWith->setChecked(_settings.value("FAR.FindTxt.S", false).toBool());
	m_findTextContains->setChecked(_settings.value("FAR.FindTxt.H", true).toBool());
	m_findTextEndsWith->setChecked(_settings.value("FAR.FindTxt.E", false).toBool());
	m_findTextRegex->setChecked(_settings.value("FAR.FindTxt.R", false).toBool());

	return m_centralSplitter;
}

bool FAR::prepareToolShutdown(QSettings& _settings) {
	_settings.setValue("FAR.RootDir", m_rootDir->text());
	_settings.setValue("FAR.FindTxt", m_findText->text());
	_settings.setValue("FAR.FindTxt.C", m_findTextCaseSensitive->isChecked());
	_settings.setValue("FAR.FindTxt.S", m_findTextStartsWith->isChecked());
	_settings.setValue("FAR.FindTxt.H", m_findTextContains->isChecked());
	_settings.setValue("FAR.FindTxt.E", m_findTextEndsWith->isChecked());
	_settings.setValue("FAR.FindTxt.R", m_findTextRegex->isChecked());

	this->saveFilterGroup(m_whitelistFiles, "FAR.WLF", _settings);
	this->saveFilterGroup(m_blacklistFiles, "FAR.BLF", _settings);
	this->saveFilterGroup(m_whitelistDirectories, "FAR.WLD", _settings);
	this->saveFilterGroup(m_blacklistDirectories, "FAR.BLD", _settings);

	return true;
}

void FAR::slotBrowseRoot(void) {
	m_rootDir->setText(QFileDialog::getExistingDirectory(m_centralSplitter, "Select root browse directory", m_rootDir->text(), QFileDialog::ShowDirsOnly));
}

void FAR::slotFindText(void) {
	FARFilter filter;
	this->setupFilter(filter);

	this->slotLock();

	FARFilter::FilterFlag textFilter = FARFilter::NoFlags;
	if (m_findTextCaseSensitive->isChecked()) textFilter |= FARFilter::CheckCase;
	if (m_findTextStartsWith->isChecked()) textFilter |= FARFilter::StartsWith;
	if (m_findTextContains->isChecked()) textFilter |= FARFilter::Contains;
	if (m_findTextEndsWith->isChecked()) textFilter |= FARFilter::EndsWith;
	if (m_findTextRegex->isChecked()) textFilter |= FARFilter::AsRegex;

	std::thread t(&FAR::workerFindText, this, m_rootDir->text(), m_findText->text(), textFilter, filter);
	t.detach();
}

void FAR::slotLock(void) {
	m_centralSplitter->setEnabled(false);
}

void FAR::slotUnlock(void) {
	m_centralSplitter->setEnabled(true);
}

void FAR::setupFilter(FARFilter& _filter) const {
	_filter.setWhitelistFiles(m_whitelistFiles.edit->toPlainText().split("\n", Qt::SkipEmptyParts));
	_filter.setWhitelistFilesFlags(this->filterFlagsFromGroup(m_whitelistFiles));
	_filter.setBlacklistFiles(m_blacklistFiles.edit->toPlainText().split("\n", Qt::SkipEmptyParts));
	_filter.setBlacklistFilesFlags(this->filterFlagsFromGroup(m_blacklistFiles));
	_filter.setWhitelistDirectories(m_whitelistDirectories.edit->toPlainText().split("\n", Qt::SkipEmptyParts));
	_filter.setWhitelistDirectoriesFlags(this->filterFlagsFromGroup(m_whitelistDirectories));
	_filter.setBlacklistDirectories(m_blacklistDirectories.edit->toPlainText().split("\n", Qt::SkipEmptyParts));
	_filter.setBlacklistDirectoriesFlags(this->filterFlagsFromGroup(m_blacklistDirectories));
}

FARFilter::FilterFlag FAR::filterFlagsFromGroup(const FilterGroup& _group) const {
	FARFilter::FilterFlag flags = FARFilter::NoFlags;
	if (_group.checkCase->isChecked()) flags |= FARFilter::CheckCase;
	if (_group.starts->isChecked()) flags |= FARFilter::StartsWith;
	if (_group.contains->isChecked()) flags |= FARFilter::Contains;
	if (_group.ends->isChecked()) flags |= FARFilter::EndsWith;
	if (_group.regex->isChecked()) flags |= FARFilter::AsRegex;
	return flags;
}

FAR::FilterGroup FAR::createFilterGroup(const QString& _labelText, const QString& _settingsKey, QSettings& _settings, QGridLayout* _layout, int _row) {
	FAR::FilterGroup group;
	group.label = new QLabel(_labelText);
	group.edit = new QPlainTextEdit;
	group.layout = new QVBoxLayout;
	group.edit->setPlainText(_settings.value(_settingsKey, QString()).toString());
	group.checkCase = new QCheckBox("Case sensitive");
	group.checkCase->setChecked(_settings.value(_settingsKey + ".C", false).toBool());
	group.starts = new QCheckBox("Starts with");
	group.starts->setChecked(_settings.value(_settingsKey + ".S", false).toBool());
	group.starts->setToolTip("If active the entries will be checked as path starts with. If no option selected the filter will be ignored.");
	group.starts->setToolTipDuration(5000);
	group.contains = new QCheckBox("Contains");
	group.contains->setChecked(_settings.value(_settingsKey + ".H", false).toBool());
	group.contains->setToolTip("If active the entries will be checked as path contains. If no option selected the filter will be ignored.");
	group.contains->setToolTipDuration(5000);
	group.ends = new QCheckBox("Ends with");
	group.ends->setChecked(_settings.value(_settingsKey + ".E", false).toBool());
	group.ends->setToolTip("If active the entries will be checked as path ends with. If no option selected the filter will be ignored.");
	group.ends->setToolTipDuration(5000);
	group.regex = new QCheckBox("Regex");
	group.regex->setChecked(_settings.value(_settingsKey + ".R", false).toBool());
	group.regex->setToolTip("If active the entries will interpreted as a regex. If no option selected the filter will be ignored.");
	group.regex->setToolTipDuration(5000);

	group.layout->addWidget(group.checkCase);
	group.layout->addWidget(group.starts);
	group.layout->addWidget(group.contains);
	group.layout->addWidget(group.ends);
	group.layout->addWidget(group.regex);

	_layout->addWidget(group.label, _row, 0, Qt::AlignTop | Qt::AlignRight);
	_layout->addWidget(group.edit, _row, 1);
	_layout->addLayout(group.layout, _row, 3);
	
	return group;
}

void FAR::saveFilterGroup(const FilterGroup& _group, const QString& _settingsKey, QSettings& _settings) {
	_settings.setValue(_settingsKey, _group.edit->toPlainText());
	_settings.setValue(_settingsKey + ".C", _group.checkCase->isChecked());
	_settings.setValue(_settingsKey + ".S", _group.starts->isChecked());
	_settings.setValue(_settingsKey + ".H", _group.contains->isChecked());
	_settings.setValue(_settingsKey + ".E", _group.ends->isChecked());
	_settings.setValue(_settingsKey + ".R", _group.regex->isChecked());
}

void FAR::workerFindText(QString _root, QString _text, FARFilter::FilterFlag _textFilter, FARFilter _filter) {
	this->workerFindTextLogic(_root, _text, _textFilter, _filter);
	QMetaObject::invokeMethod(this, &FAR::slotUnlock, Qt::QueuedConnection);
}

void FAR::workerFindTextLogic(const QString& _root, const QString& _text, FARFilter::FilterFlag _textFilter, const FARFilter& _filter) {
	qint64 beginTime = QDateTime::currentMSecsSinceEpoch();

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
	if (!root.findText(_text, _textFilter, matches, longestText, longestPath, false)) {
		FAR_LOGE("> Search cancelled.");
		return;
	}

	QString pathSuffix(" @ line ");
	longestPath = longestPath + pathSuffix.length() + 11;

	// Create filler string
	char* fillBufferC = new char[longestPath + 1];
	for (int i = 0; i < longestPath; i++) fillBufferC[i] = ' ';
	fillBufferC[longestPath] = 0;

	QString msg("Matches:\n");

	int tmp = 0;
	for (FARMatch& match : matches) {
		msg.append(match.fullPath());
		msg.append(pathSuffix);
		msg.append(QString::number(match.line()));
		msg.append(':');
		msg.append(QString::fromLocal8Bit(fillBufferC, longestPath - (match.fullPath().length() + pathSuffix.length() + QString::number(match.line()).length() + 1)));
		msg.append(QString(match.text()).trimmed());
		msg.append('\n');
	}

	FAR_LOG(msg);

	// Calculate time and print
	qint64 endTime = QDateTime::currentMSecsSinceEpoch();
	QString diff = QString::number((endTime - beginTime) / 1000) + ".";
	QString msdiff = QString::number((endTime - beginTime) % 1000);
	while (msdiff.length() < 4) msdiff.push_front('0');

	FAR_LOG("\"" FAR_SEARCHMODE_FindText "\" ended with " + QString::number(matches.size()) + " matches (took " + diff + msdiff + " seconds)");

}