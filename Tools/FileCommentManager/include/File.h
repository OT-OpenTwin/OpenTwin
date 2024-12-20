#pragma once

// Project header
#include "ScanData.h"
#include "ScanOptions.h"

// Qt header
#include <QtCore/qfile.h>

class File {
public:
	enum FileType {
		Header,
		Source
	};
	
	File(const QString& _filePath, FileType _type);

	bool process(const ScanData& _scanData, const ScanOptions& _options);

private:
	QString m_path;
	FileType m_type;

	File() = delete;
	File(const File&) = delete;
	File(File&&) = delete;
	File& operator = (const File&) = delete;
	File& operator = (File&&) = delete;
};