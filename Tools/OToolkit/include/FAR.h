//! @file FAR.h
//! @author Alexander Kuester (alexk95)
//! @date November 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// Toolkit API header
#include "OTCore/OTClassHelper.h"
#include "OTCore/Flags.h"
#include "OToolkitAPI/Tool.h"

// Qt header
#include <QtCore/qstringlist.h>

// std header
#include <list>

class QLabel;
class QWidget;
class QSplitter;
class QGroupBox;
class QCheckBox;
class QLineEdit;
class QTabWidget;
class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QPlainTextEdit;
class QPushButton;

class FARMatch {
public:
	FARMatch() : m_line(0) {};
	FARMatch(const QString& _fullPath, int _line, const QString& _text) : m_fullPath(_fullPath), m_line(_line), m_text(_text) {};
	FARMatch(const FARMatch& _other) : m_text(_other.m_text), m_fullPath(_other.m_fullPath), m_line(_other.m_line) {};

	void setText(const QString& _text) { m_text = _text; };
	const QString& text(void) const { return m_text; };

	void setLine(int _line) { m_line = _line; };
	int line(void) const { return m_line; };

	void setFullPath(const QString& _path) { m_fullPath = _path; };
	const QString& fullPath(void) const { return m_fullPath; };

private:
	QString m_text;
	int m_line;
	QString m_fullPath;
};

class FARFile;
class FARDirectory;

class FARFilter {
public:
	enum FilterFlag {
		NoFlags    = 0x00,
		StartsWith = 0x01,
		Contains   = 0x02,
		EndsWith   = 0x04,
		AsRegex    = 0x08,
		CheckCase  = 0x10
	};

	FARFilter();
	FARFilter(const FARFilter& _other);

	bool validate(const FARFile& _file) const;
	bool validate(const FARDirectory& _dir) const;

	void setWhitelistFiles(const QStringList& _whitelist) { m_whitelistFiles = _whitelist; };
	const QStringList& whitelistFiles(void) const { return m_whitelistFiles; };

	void setWhitelistFilesFlags(FilterFlag _flags) { m_whitelistFilesFlags = _flags; };
	FilterFlag whitelistFilesFlags(void) const { return m_whitelistFilesFlags; };

	void setBlacklistFiles(const QStringList& _blacklist) { m_blacklistFiles = _blacklist; };
	const QStringList& blacklistFiles(void) const { return m_blacklistFiles; };

	void setBlacklistFilesFlags(FilterFlag _flags) { m_whitelistFilesFlags = _flags; };
	FilterFlag blacklistFilesFlags(void) const { return m_whitelistFilesFlags; };

	void setWhitelistDirectories(const QStringList& _whitelist) { m_whitelistDirectories = _whitelist; };
	const QStringList& whitelistDirectories(void) const { return m_whitelistDirectories; };

	void setWhitelistDirectoriesFlags(FilterFlag _flags) { m_whitelistFilesFlags = _flags; };
	FilterFlag whitelistDirectoriesFlags(void) const { return m_whitelistFilesFlags; };

	void setBlacklistDirectories(const QStringList& _blacklist) { m_blacklistDirectories = _blacklist; };
	const QStringList& blacklistDirectories(void) const { return m_blacklistDirectories; };

	void setBlacklistDirectoriesFlags(FilterFlag _flags) { m_whitelistFilesFlags = _flags; };
	FilterFlag blacklistDirectoriesFlags(void) const { return m_whitelistFilesFlags; };

private:
	QStringList m_whitelistFiles;
	FilterFlag m_whitelistFilesFlags;
	QStringList m_blacklistFiles;
	FilterFlag m_blacklistFilesFlags;
	QStringList m_whitelistDirectories;
	FilterFlag m_whitelistDirectoriesFlags;
	QStringList m_blacklistDirectories;
	FilterFlag m_blacklistDirectoriesFlags;
};

class FARFile {
public:

	FARFile(const QString& _fullPath);
	FARFile(const FARFile& _other);

	bool exists(void) const;

	bool readAll(void);

	bool writeAll(void);

	bool findText(const QString& _text, FARFilter::FilterFlag _textFilter, std::list<FARMatch>& _matches, int& _longestText, int& _longestPath) const;

	bool replaceText(const QString& _text, const QString& _newText, FARFilter::FilterFlag _textFilter, std::list<FARMatch>& _matches, int& _longestText, int& _longestPath);

	void setFullPath(const QString& _fullPath) { m_fullPath = _fullPath; };
	const QString& fullPath(void) const { return m_fullPath; };

	void setNameOnly(const QString& _name) { m_nameOnly = _name; };
	const QString& nameOnly(void) const { return m_nameOnly; };

	void setFileExtension(const QString& _extension) { m_fileExtension = _extension; };
	const QString& fileExtension(void) const { return m_fileExtension; };

	void setTextLines(const QStringList& _lines) { m_textLines = _lines; };
	const QStringList& textLines(void) const { return m_textLines; };

private:
	QString m_fullPath;
	QString m_nameOnly;
	QString m_fileExtension;
	QStringList m_textLines;
};

class FARDirectory {
public:

	FARDirectory(const QString& _fullPath);
	FARDirectory(const FARDirectory& _parentDirectory, const QString& _directoryName);
	FARDirectory(const FARDirectory& _other);

	bool exists(void) const;

	//! @brief Find all files in this directory.
	//! Result is buffered.
	bool scanFiles(const FARFilter& _filter, int& _counter);

	//! @brief Read all the files found in this directory.
	//! Note that scan files must be called first if the files are not set manually.
	//! Result is buffered.
	//! @param _topLevelOnly If true only the files in this directory will read their contents
	bool readAllFiles(bool _topLevelOnly = true);

	//! @brief Find all the child directories in this directory.
	//! Result is buffered.
	//! @param _topLevelOnly If true only this directory will be scanned
	bool scanDirectories(const FARFilter& _filter, int& _counter, bool _topLevelOnly = true);

	//! @brief Find all files and directories in this directory.
	//! Result is buffered.
	//! @param _topLevelOnly If true only this directory will be scanned
	//! @param _readFiles If true the found files will read their text
	bool scanAll(const FARFilter& _filter, int& _fileCounter, int& _directoryCounter, bool _topLevelOnly = true, bool _readFiles = false);

	bool findText(const QString& _text, FARFilter::FilterFlag _textFilter, std::list<FARMatch>& _matches, int& _longestText, int& _longestPath, bool _topLevelOnly = true) const;

	bool replaceText(const QString& _text, const QString& _newText, FARFilter::FilterFlag _textFilter, std::list<FARMatch>& _matches, int& _longestText, int& _longestPath, bool _topLevelOnly = true);

	void setFullPath(const QString& _path) { m_fullPath = _path; };
	const QString& fullPath(void) const { return m_fullPath; };

	void setNameOnly(const QString& _name) { m_nameOnly = _name; };
	const QString& nameOnly(void) const { return m_nameOnly; };

	void setFiles(const std::list<FARFile>& _files) { m_files = _files; };
	const std::list<FARFile>& files(void) const { return m_files; };

	void setDirectories(const std::list<FARDirectory>& _directories) { m_directories = _directories; };
	const std::list<FARDirectory>& directories(void) const { return m_directories; };

private:
	QString m_fullPath;
	QString m_nameOnly;
	std::list<FARFile> m_files;
	std::list<FARDirectory> m_directories;
};

class FAR : public QObject, public otoolkit::Tool {
	Q_OBJECT
public:
	FAR();
	virtual ~FAR();

	// ###########################################################################################################################################################################################################################################################################################################################

	// API base functions

	//! @brief Return the unique tool name
	//! The name will be used to create all required menu entries
	virtual QString toolName(void) const override;

	//! @brief Create the central widget that will be displayed to the user in the main tab view
	virtual QWidget* runTool(QMenu* _rootMenu, std::list<QWidget*>& _statusWidgets) override;

	virtual void restoreToolSettings(QSettings& _settings) override;

	//! @brief Stop all the logic of this tool
	virtual bool prepareToolShutdown(QSettings& _settings) override;

private Q_SLOTS:
	void slotBrowseRoot(void);
	void slotFindText(void);
	void slotReplaceText(void);
	void slotLock(void);
	void slotUnlock(void);

private:
	struct FilterGroup {
		QLabel* label;
		QPlainTextEdit* edit;
		QCheckBox* checkCase;
		QCheckBox* starts;
		QCheckBox* contains;
		QCheckBox* ends;
		QCheckBox* regex;
		QVBoxLayout* layout;
	};

	//! @brief Setup the provided filter from the current configuration
	void setupFilter(FARFilter& _filter) const;
	FARFilter::FilterFlag filterFlagsFromGroup(const FilterGroup& _group) const;

	void workerFindText(QString _root, QString _text, FARFilter::FilterFlag _textFilter, FARFilter _filter);
	void workerFindTextLogic(const QString& _root, const QString& _text, FARFilter::FilterFlag _textFilter, const FARFilter& _filter);

	void workerReplaceText(QString _root, QString _text, QString _newText, FARFilter::FilterFlag _textFilter, FARFilter _filter);
	void workerReplaceTextLogic(const QString& _root, const QString& _text, const QString& _newText, FARFilter::FilterFlag _textFilter, FARFilter _filter);

	FilterGroup createFilterGroup(const QString& _labelText, QGridLayout* _layout, int _row);
	void restoreFilterGroupSettings(FilterGroup& _group, const QString& _settingsKey, QSettings& _settings);

	void saveFilterGroup(const FilterGroup& _group, const QString& _settingsKey, QSettings& _settings);

	QSplitter* m_centralSplitter;

	QWidget* m_leftGridW;
	QGridLayout* m_leftGrid;
	
	QLabel* m_leftTitle;

	

	FilterGroup m_whitelistFiles;
	FilterGroup m_blacklistFiles;
	FilterGroup m_whitelistDirectories;
	FilterGroup m_blacklistDirectories;

	QWidget* m_rightLayoutW;
	QVBoxLayout* m_rightLayout;

	QHBoxLayout* m_rightTopLayout;

	QLabel* m_rootDirL;
	QLineEdit* m_rootDir;
	QPushButton* m_browseRootDir;

	QTabWidget* m_findModeTab;

	QWidget* m_findTextLayoutW;
	QVBoxLayout* m_findTextLayout;
	QGridLayout* m_findTextTopLayout;
	QHBoxLayout* m_findTextButtonLayout;
	QLabel* m_findTextL;
	QLineEdit* m_findText;
	QLabel* m_findTextMode;
	QCheckBox* m_findTextCaseSensitive;
	QCheckBox* m_findTextStartsWith;
	QCheckBox* m_findTextContains;
	QCheckBox* m_findTextEndsWith;
	QCheckBox* m_findTextRegex;
	QPushButton* m_findTextBtn;

	QWidget* m_replaceTextLayoutW;
	QVBoxLayout* m_replaceTextLayout;
	QGridLayout* m_replaceTextTopLayout;
	QHBoxLayout* m_replaceTextButtonLayout;
	QLabel* m_replaceTextL;
	QLineEdit* m_replaceText;
	QCheckBox* m_replaceTextCaseSensitive;
	QLabel* m_replaceWithTextL;
	QLineEdit* m_replaceWithText;
	QPushButton* m_replaceTextBtn;
};

OT_ADD_FLAG_FUNCTIONS(FARFilter::FilterFlag)
