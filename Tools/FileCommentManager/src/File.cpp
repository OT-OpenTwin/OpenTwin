// Project header
#include "File.h"
#include "Logger.h"

// Qt header
#include <QtCore/qfile>

#define LOG_I(___message) LOG_INFO("File", ___message)
#define LOG_W(___message) LOG_WARNING("File", ___message)
#define LOG_E(___message) LOG_ERROR("File", ___message)

File::File(const QString& _filePath, FileType _type) 
	: m_path(_filePath), m_type(_type)
{

}
