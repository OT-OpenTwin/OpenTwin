// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/CoreTypes.h"

// Qt header
#include <QtCore/qstring.h>

class LogInGSSEntry {
	OT_DECL_DEFCOPY(LogInGSSEntry)
	OT_DECL_DEFMOVE(LogInGSSEntry)
public:
	LogInGSSEntry() = default;
	LogInGSSEntry(const QString& _name, const QString& _url, const QString& _port);

	void setName(const QString& _name) { m_name = _name; };
	const QString& getName(void) const { return m_name; };

	void setUrl(const QString& _url) { m_url = _url; };
	const QString& getUrl(void) const { return m_url; };

	void setPort(const QString& _port) { m_port = _port; };
	const QString& getPort(void) const { return m_port; };

	QString getConnectionUrl(void) const { return m_url + ":" + m_port; };

	void clear(void);

	bool isValid(void) const { return !m_name.isEmpty() && !m_url.isEmpty() && !m_port.isEmpty(); };

	QString getDisplayText(void) const { return m_name + " (" + m_url + ":" + m_port + ")"; };

private:
	QString		m_name;
	QString		m_url;
	QString		m_port;
};