// Project header
#include "ScanData.h"

ScanData::ScanData(const std::map<QString, Project*>& _projects, const std::map<QString, QString>& _hashData)
	: m_projects(_projects), m_fileHashData(_hashData)
{

}
