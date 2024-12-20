#pragma once

// Qt header
#include <QtCore/qstringlist.h>

class Config {
public:
	static Config& instance(void) {
		static Config g_instance;
		return g_instance;
	};

	bool readFromFile(const QString& _filePath, const QString& _devRoot);

	void setUser(const QString& _user) { m_user = _user; };
	const QString& getUser(void) const { return m_user; };
	const QStringList& getProjectSearchPaths(void) const { return m_projectSearchPaths; };

private:
	Config();
	~Config() {}

	QString m_user;
	QStringList m_projectSearchPaths;
	QStringList m_fileExtensions;

	Config(const Config& _other);
	Config(Config&& _other);
	Config& operator = (const Config&) = delete;
	Config& operator = (Config&&) = delete;
};