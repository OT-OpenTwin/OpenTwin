//! @file LogInGSSEntry.h
//! @author Alexander Kuester (alexk95)
//! @date June 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// Qt header
#include <QtCore/qstring.h>

class LogInGSSEntry {
public:
	LogInGSSEntry();
	LogInGSSEntry(const QString& _name, const QString& _url, const QString& _port);
	LogInGSSEntry(const LogInGSSEntry& _other);

	LogInGSSEntry& operator = (const LogInGSSEntry& _other);

	void setName(const QString& _name) { m_name = _name; };
	const QString& getName(void) const { return m_name; };

	void setUrl(const QString& _url) { m_url = _url; };
	const QString& getUrl(void) const { return m_url; };

	void setPort(const QString& _port) { m_port = _port; };
	const QString& getPort(void) const { return m_port; };

	QString getDisplayText(void) const { return m_name + " (" + m_url + ":" + m_port + ")"; };

private:
	QString		m_name;
	QString		m_url;
	QString		m_port;
};