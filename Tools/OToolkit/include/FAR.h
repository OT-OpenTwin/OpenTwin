//! @file FAR.h
//! @author Alexander Kuester (alexk95)
//! @date November 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// Toolkit API header
#include "OTCore/OTClassHelper.h"
#include "OToolkitAPI/Tool.h"

// Qt header
#include <QtCore/qstringlist.h>

// std header
#include <list>

class QLabel;
class QWidget;
class QSplitter;
class QLineEdit;
class QTabWidget;
class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QPlainTextEdit;
class QPushButton;

class FARMatch {

	OT_PROPERTY_REF(QString, text, setText, text)
	OT_PROPERTY(int, line, setLine, line)
	OT_PROPERTY_REF(QString, fullPath, setFullPath, fullPath)

public:
	FARMatch() : m_line(0) {};
	FARMatch(const QString& _fullPath, int _line, const QString& _text) : m_fullPath(_fullPath), m_line(_line), m_text(_text) {};
	FARMatch(const FARMatch& _other) : m_text(_other.m_text), m_fullPath(_other.m_fullPath), m_line(_other.m_line) {};
};

class FARFile;
class FARDirectory;

class FARFilter {

	OT_PROPERTY_REF(QStringList, whitelistFiles, setWhitelistFiles, whitelistFiles)
	OT_PROPERTY_REF(QStringList, blacklistFiles, setBlacklistFiles, blacklistFiles)
	OT_PROPERTY_REF(QStringList, whitelistDirectories, setWhitelistDirectories, whitelistDirectories)
	OT_PROPERTY_REF(QStringList, blacklistDirectories, setBlacklistDirectories, blacklistDirectories)

public:
	FARFilter() {};
	FARFilter(const FARFilter& _other) : m_whitelistDirectories(_other.m_whitelistDirectories), m_blacklistDirectories(_other.m_blacklistDirectories), m_blacklistFiles(_other.m_blacklistFiles), m_whitelistFiles(_other.m_whitelistFiles) {};

	bool validate(const FARFile& _file) const;
	bool validate(const FARDirectory& _dir) const;

};

class FARFile {

	OT_PROPERTY_REF(QString, fullPath, setFullPath, fullPath)
	OT_PROPERTY_REF(QString, nameOnly, setNameOnly, nameOnly)
	OT_PROPERTY_REF(QString, fileExtension, setFileExtension, fileExtension)
	OT_PROPERTY_REF(QStringList, textLines, setTextLines, textLines)

public:

	FARFile(const QString& _fullPath);
	FARFile(const FARFile& _other);

	bool exists(void) const;

	bool readAll(void);

	bool findText(const QString& _text, std::list<FARMatch>& _matches, int& _longestText, int& _longestPath) const;

};

class FARDirectory {

	OT_PROPERTY_REF(QString, fullPath, setFullPath, fullPath)
	OT_PROPERTY_REF(QString, nameOnly, setNameOnly, nameOnly)
	OT_PROPERTY_REF(std::list<FARFile>, files, setFiles, files)
	OT_PROPERTY_REF(std::list<FARDirectory>, directories, setDirectories, directories)

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

	bool findText(const QString& _text, std::list<FARMatch>& _matches, int& _longestText, int& _longestPath, bool _topLevelOnly = true) const;

private:
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
	virtual QWidget* runTool(QMenu* _rootMenu, std::list<QWidget*>& _statusWidgets, QSettings& _settings) override;

	//! @brief Stop all the logic of this tool
	virtual bool prepareToolShutdown(QSettings& _settings) override;

private slots:
	void slotBrowseRoot(void);
	void slotFindText(void);
	void slotLock(void);
	void slotUnlock(void);

private:
	//! @brief Setup the provided filter from the current configuration
	void setupFilter(FARFilter& _filter) const;

	void workerFindText(QString _root, QString _text, FARFilter _filter);
	void workerFindTextLogic(const QString& _root, const QString& _text, const FARFilter& _filter);

	QSplitter* m_centralSplitter;

	QWidget* m_leftGridW;
	QGridLayout* m_leftGrid;
	
	QLabel* m_leftTitle;
	QLabel* m_whitelistFilesL;
	QPlainTextEdit* m_whitelistFiles;
	QLabel* m_blacklistFilesL;
	QPlainTextEdit* m_blacklistFiles;
	QLabel* m_whitelistDirectoriesL;
	QPlainTextEdit* m_whitelistDirectories;
	QLabel* m_blacklistDirectoriesL;
	QPlainTextEdit* m_blacklistDirectories;

	QWidget* m_rightLayoutW;
	QVBoxLayout* m_rightLayout;

	QHBoxLayout* m_rightTopLayout;

	QLabel* m_rootDirL;
	QLineEdit* m_rootDir;
	QPushButton* m_browseRootDir;

	QTabWidget* m_findModeTab;

	QWidget* m_findTextLayoutW;
	QVBoxLayout* m_findTextLayout;
	QHBoxLayout* m_findTextTopLayout;
	QHBoxLayout* m_findTextButtonLayout;
	QLabel* m_findTextL;
	QLineEdit* m_findText;
	QPushButton* m_findTextBtn;

};