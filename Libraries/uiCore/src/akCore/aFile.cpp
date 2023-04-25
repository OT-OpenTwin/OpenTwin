/*
 *	File:		aFile.cpp
 *	Package:	akCore
 *
 *  Created on: September 28, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

// AK header
#include <akCore/aFile.h>		// Corresponding header
#include <akCore/aException.h>	// Error handling

// Qt header
#include <qfile.h>

ak::aFile::aFile()
	: m_changed(false), m_extension(), m_lines(), m_name(), m_path(), m_uid(ak::invalidUID)
{}

ak::aFile::~aFile() {}

// ####################################################################################

// Setter

void ak::aFile::setUid(
	ak::UID					_uid
) { m_uid = _uid; }

void ak::aFile::load(
	const QString &			_filePath
) {
	try {
		if (_filePath.length() > 0) { setPath(_filePath); }
		importFile();
	}
	catch (const aException & e) { throw aException(e, "ak::aFile::load()"); }
	catch (const std::exception & e) { throw aException(e.what(), "ak::aFile::load()"); }
	catch (...) { throw aException("Unknown error", "ak::aFile::load()"); }
}

void ak::aFile::save(
	bool					_append
) { exportFile(_append); }

void ak::aFile::save(
	const QString &			_filePath,
	bool					_append
) { setPath(_filePath); exportFile(_append); }

void ak::aFile::setPath(
	const QString &			_path
) {
	m_path = _path;
	rebuildPath();

	// Filter file name and extension
	QStringList fileName = m_path.split("/", QString::SplitBehavior::KeepEmptyParts);
	QStringList extension = fileName.at(fileName.count() - 1).split(".");
	m_name = extension.at(0);
	for (int i = 1; i < extension.count() - 1; i++) {
		m_name.append('.'); m_name.append(extension.at(i));
	}
	m_extension = extension.at(extension.count() - 1);
}

void ak::aFile::setLines(
	const QStringList &		_lines
) { m_lines = _lines; m_changed = true; }

void ak::aFile::addLine(
	const QString &			_line
) { m_lines.push_back(_line); m_changed = true; }

void ak::aFile::addLine(
	const QStringList &		_lines
) { m_lines.append(_lines); m_changed = true; }

// ####################################################################################

// Getter

ak::UID ak::aFile::uid(void) const { return m_uid; }

QString ak::aFile::name() const { return m_name; }

QString ak::aFile::path() const { return m_path; }

QString ak::aFile::extension() const { return m_extension; }

QStringList ak::aFile::lines() const { return m_lines; }

int ak::aFile::linesCount() const { return m_lines.count(); }

bool ak::aFile::hasChanged() const { return m_changed; }

// ####################################################################################

// Private members

void ak::aFile::rebuildPath() {
	QStringList fileName = m_path.split("\\", QString::SplitBehavior::SkipEmptyParts);
	m_path = fileName.at(0);

	// Rebuild path
	for (int i = 1; i < fileName.count(); i++) { m_path.append('/'); m_path.append(fileName.at(i)); }
}

void ak::aFile::importFile() {
	try {
		// Check file
		QFile f(m_path);
		if (!f.exists()) { assert(0); throw aException("The provided file deos not exist", "Check file"); }

		// Open file
		if (!f.open(QIODevice::ReadOnly)) { assert(0); throw aException("Failed to open file for reading", "Open file"); }

		// Read file
		m_lines.clear();
		while (!f.atEnd()) { QString line = f.readLine(); m_lines.push_back(line); }
		f.close();
	}
	catch (const aException & e) { throw aException(e, "ak::aFile::importFile()"); }
	catch (const std::exception & e) { throw aException(e.what(), "ak::aFile::importFile()"); }
	catch (...) { throw aException("Unknown error", "ak::aFile::importFile()"); }
}

void ak::aFile::exportFile(
	bool					_append
) const {
	try {
		QFile f(m_path);
		if (_append) {
			if (!f.open(QIODevice::OpenModeFlag::Append)) { assert(0); throw aException("Failed to open file", "Check file"); }
		}
		else {
			if (!f.open(QIODevice::OpenModeFlag::WriteOnly)) { assert(0); throw aException("Failed to open file", "Check file"); }
		}
		for (QString str : m_lines) {
			std::string s(str.toStdString());
			f.write(s.c_str());
		}
		f.close();
	}
	catch (const aException & e) { throw aException(e, "ak::aFile::importFile()"); }
	catch (const std::exception & e) { throw aException(e.what(), "ak::aFile::importFile()"); }
	catch (...) { throw aException("Unknown error", "ak::aFile::importFile()"); }
}