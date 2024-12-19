#pragma once

// Qt header
#include <QtCore/qfile.h>

class File {
public:
	enum FileType {
		Header,
		Source
	};
	
	File(const QString& _filePath, FileType _type);

private:
	QString m_path;
	FileType m_type;

	File() = delete;
	File(const File&) = delete;
	File(File&&) = delete;
	File& operator = (const File&) = delete;
	File& operator = (File&&) = delete;
};