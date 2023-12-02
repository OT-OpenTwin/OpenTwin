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

class QSplitter;

class FARFile {

	OT_PROPERTY_REF(QString, fullPath, setFullPath, fullPath)
	OT_PROPERTY_REF(QString, nameOnly, setNameOnly, nameOnly)
	OT_PROPERTY_REF(QString, fileExtension, setFileExtension, fileExtension)

public:

	FARFile(const QString& _fullPath);
	FARFile(const FARFile& _other);

	void findLineContains(const QString& _line);

};

class FARDirectory {

	OT_PROPERTY_REF(QString, fullPath, setFullPath, fullPath);
	OT_PROPERTY_REF(QString, nameOnly, setNameOnly, nameOnly);

public:

	FARDirectory(const QString& _fullPath);
	FARDirectory(const FARDirectory& _parentDirectory, const QString& _directoryName);
	FARDirectory(const FARDirectory& _other);

	std::list<FARFile> findFiles(const QStringList& _whitelist, const QStringList& _blacklist);
	std::list<FARDirectory> findDirectories(const QStringList& _whitelist, const QStringList& _blacklist);

private:
	bool isEntryOk(const QString& _entry, const QStringList& _whitelist, const QStringList& _blacklist) const;
};

class FAR : public otoolkit::Tool {
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

private:

	QSplitter* m_centralSplitter;

};