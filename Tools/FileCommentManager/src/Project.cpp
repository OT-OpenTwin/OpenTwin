// Project header
#include "File.h"
#include "Logger.h"
#include "AppBase.h"
#include "Project.h"
#include "ScanData.h"

// Qt header
#include <QtCore/qfile.h>
#include <QtCore/qxmlstream.h>
#include <QtCore/qtextstream.h>

#define LOG_I(___message) LOG_INFO("Project", ___message)
#define LOG_W(___message) LOG_WARNING("Project", ___message)
#define LOG_E(___message) LOG_ERROR("Project", ___message)

Project::Project() {

}

bool Project::scan(const QString& _path, const QString& _projectName) {
	m_path = _path;
	m_projectName = _projectName;

    // Open project file for reading
	QFile projectFile(m_path + "/" + m_projectName + ".vcxproj");
	if (!projectFile.open(QIODevice::ReadOnly)) {
		LOG_E("Failed to open file for reading: \"" + projectFile.fileName() + "\"");
		return false;
	}

    // Parse project file
	QXmlStreamReader reader(&projectFile);
    try {

        //Parse the XML file
        while (!reader.atEnd() && !reader.hasError()) {
            // Read next token

            QXmlStreamReader::TokenType token = reader.readNext();

            // Read start element token
            if (token == QXmlStreamReader::StartElement) {
                // Current element is compile or include element
                if (reader.name() == QStringLiteral("ClCompile") || reader.name() == QStringLiteral("ClInclude")) {
                    // Read all attributes of current element and search for include attribute
                    for (const QXmlStreamAttribute& attribute : reader.attributes()) {
                        if (attribute.name() == QStringLiteral("Include")) {
                            QString filePath = attribute.value().toString();
                            // Add source
                            if (reader.name() == QStringLiteral("ClCompile")) {
                                m_sources.push_back(new File(m_path + "/" + filePath, File::Source));
                            }
                            // Add header
                            else if (reader.name() == QStringLiteral("ClInclude")) {
                                m_headers.push_back(new File(m_path + "/" + filePath, File::Header));
                            }
                        }
                    }
                }
            }
        }

        projectFile.close();
    }
    catch (const std::exception& _e) {
        LOG_E(_e.what());
        projectFile.close();
        return false;
    }
    catch (...) {
        LOG_E("Unknown error");
        projectFile.close();
        return false;
    }


    // Open cache file
    QFile cacheFile(m_path + "/" + m_projectName + ".otfcm.cache");
    if (cacheFile.exists()) {
        if (!cacheFile.open(QIODevice::ReadOnly)) {
            LOG_E("Failed to open cache file for reading: \"" + m_path + "/" + m_projectName + ".otfcm.cache\"");
            return false;
        }

        // Read cache file
        QTextStream cacheStream(&cacheFile);
        while (!cacheStream.atEnd()) {
            QStringList lineData = cacheStream.readLine().split('|');
            if (lineData.count() == 2) {
                m_fileHashData.insert_or_assign(lineData[0], lineData[1]);
            }
        }
        
        cacheFile.close();
    }

    return true;
}

bool Project::scanFiles(const AppBase* _app, const ScanOptions& _options) {
    ScanData scanData(_app->getProjects(), m_fileHashData);

    for (File* file : m_headers) {
        if (!file->process(scanData, _options)) {

        }
    }

    return true;
}
