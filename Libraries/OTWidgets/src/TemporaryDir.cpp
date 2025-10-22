// @otlicense

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTWidgets/TemporaryDir.h"

// Qt header
#include <QtCore/qdir.h>
#include <QtCore/qfile.h>

ot::TemporaryDir::TemporaryDir(const QString& _parentDir) 
	: m_tempDir(cleanDir(_parentDir) + "XXXXXX")
{

}

// ###########################################################################################################################################################################################################################################################################################################################

// File manipulation

bool ot::TemporaryDir::addFile(const std::string& _entityName, const QString& _fileName, const QByteArray& _data, bool _replaceExisting) {
	const QString filePath = m_tempDir.path() + QDir::separator() + _fileName;
	
	// Ensure file does not exist
	if (QFile::exists(filePath)) {
		if (!_replaceExisting) {
			return false;
		}

		if (!QFile::remove(filePath)) {
			OT_LOG_E("Failed to remove file at temporary path. Maybe the fill is still open by another application. File: \"" + filePath.toStdString() + "\"");
			return false;
		}
	}

	// Open file for writing
	QFile file(filePath);
	if (!file.open(QIODevice::WriteOnly)) {
		OT_LOG_E("Failed to open file for writing. File: \"" + filePath.toStdString() + "\"");
		return false;
	}
	qint64 bytesWritten = file.write(_data);
	file.close();

	// Ensure all data was written
	if (bytesWritten != _data.size()) {
		OT_LOG_E("Failed to write all data to file. File: \"" + filePath.toStdString() + "\"");
		QFile::remove(filePath);
		return false;
	}

	// Get new file information
	QFileInfo fileInfo(filePath);

	// Store file entry
	FileEntry entry;
	entry.entityName = _entityName;
	entry.initialFileName = _fileName;
	entry.fullPath = filePath;
	entry.lastModified = fileInfo.lastModified();

	m_files.push_back(std::move(entry));

	return true;
}

ot::TemporaryDir::FileEntry ot::TemporaryDir::getFileEntry(const std::string& _entityName) const {
	for (const auto& fileEntry : m_files) {
		if (fileEntry.entityName == _entityName) {
			return fileEntry;
		}
	}

	OT_LOG_W("File entry not found in temporary directory { \"EntityName\": \"" + _entityName + "\" }");
	return FileEntry();
}

bool ot::TemporaryDir::removeFile(const std::string& _entityName) {
	bool ok = false;

	auto it = m_files.begin();
	for (; it != m_files.end(); ) {
		if (it->entityName == _entityName) {
			QFile file(it->fullPath);
			if (!file.remove()) {
				OT_LOG_W("Failed to remove file from temporary directory { \"EntityName\": \"" + _entityName + "\", \"FilePath\": \"" + it->fullPath.toStdString() + "\" }");
				it++;
			}
			else {
				it = m_files.erase(it);
				ok = true;
			}
		}
		else {
			it++;
		}
	}

	return ok;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Information gathering

std::list<ot::TemporaryDir::FileEntry> ot::TemporaryDir::getModifiedFiles() const {
	std::list<FileEntry> lst;
	for (const auto& fileEntry : m_files) {
		QFileInfo fileInfo(fileEntry.fullPath);
		if (fileInfo.lastModified() != fileEntry.lastModified) {
			lst.push_back(fileEntry);
		}
	}
	return lst;
}

QByteArray ot::TemporaryDir::readFileData(const std::string& _entityName, bool _updateModified) {
	QByteArray data;

	// Get file entry
	FileEntry* fileEntry = getFileEntryRef(_entityName);
	if (!fileEntry) {
		return data;
	}

	// Open file for reading
	QFile file(fileEntry->fullPath);
	if (!file.open(QIODevice::ReadOnly)) {
		OT_LOG_E("Failed to open file for reading. File: \"" + fileEntry->fullPath.toStdString() + "\"");
		return data;
	}

	// Read data
	data = file.readAll();
	file.close();

	// Update modified timestamp if needed
	if (_updateModified) {
		QFileInfo fileInfo(fileEntry->fullPath);
		fileEntry->lastModified = fileInfo.lastModified();
	}

	return data;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Helper

ot::TemporaryDir::FileEntry* ot::TemporaryDir::getFileEntryRef(const std::string& _entityName) {
	for (auto& fileEntry : m_files) {
		if (fileEntry.entityName == _entityName) {
			return &fileEntry;
		}
	}

	OT_LOG_W("File entry not found in temporary directory { \"EntityName\": \"" + _entityName + "\" }");
	return nullptr;
}

QString ot::TemporaryDir::cleanDir(const QString& _dir) {
	QString dir = QDir::cleanPath(_dir);
	if (!dir.endsWith(QDir::separator())) {
		dir += QDir::separator();
	}
	return dir;
}
