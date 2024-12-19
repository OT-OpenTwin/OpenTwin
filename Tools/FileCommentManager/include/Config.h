#pragma once

// Qt header
#include <QtCore/qstringlist.h>

class Config {
public:
	static Config& instance(void);

	bool readFromFile(const QString& _filePath, const QString& _devRoot);

	const QStringList& getProjectSearchPaths(void) const { return m_projectSearchPaths; };

private:
	Config();
	~Config() {}

	QStringList m_projectSearchPaths;
	QStringList m_fileExtensions;

	Config(const Config& _other);
	Config(Config&& _other);
	Config& operator = (const Config&) = delete;
	Config& operator = (Config&&) = delete;
};