// @otlicense
// File: StartArgumentParser.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

// Frontend header
#include "AppBase.h"
#include "Helper/StartArgumentParser.h"

// OpenTwin header
#include "OTCore/String.h"
#include "OTCore/Logging/Logger.h"

// Qt header
#include <QtCore/qurl.h>
#include <QtCore/qurlquery.h>
#include <QtCore/qcommandlineparser.h>

// ###########################################################################################################################################################################################################################################################################################################################

// Parsing

bool StartArgumentParser::parse()
{
	// Reset data
	clear();

	const QStringList arguments = QCoreApplication::instance()->arguments();

	// No arguments besides the executable
	if (arguments.size() <= 1)
	{
		return true;
	}

	// The first argument after the executable determines the format.
	const QString firstArgument = arguments.at(1);

	if (firstArgument.contains("://"))
	{
		return parseUrl(firstArgument);
	}
	else
	{
		return parseCommandLine();
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Export

QStringList StartArgumentParser::createCommandLineArgs() const
{
	QStringList args;

	if (m_debug)
	{
		args << toCommandLine(ArgumentKey::Debug);
	}

	if (m_checkGraphics)
	{
		args << toCommandLine(ArgumentKey::CheckGraphics);
	}

	if (getLoginDataSet())
	{
		ot::JsonDocument doc;
		const auto& loginData = m_loginData.value();
		loginData.addRequiredDataToJson(doc, doc.GetAllocator());
		std::string encoded = ot::String::toBase64Url(doc.toJson());
		args << toCommandLine(ArgumentKey::LogInData) << QString::fromStdString(encoded);
	}

	if (getLoginGSSSet())
	{
		ot::JsonDocument doc;
		const auto& loginGSS = m_loginGSS.value();
		loginGSS.addToJsonObject(doc, doc.GetAllocator());
		std::string encoded = ot::String::toBase64Url(doc.toJson());
		args << toCommandLine(ArgumentKey::LogInGss) << QString::fromStdString(encoded);
	}

	if (getProjectInfoSet())
	{
		ot::JsonDocument doc;
		const auto& projectInfo = m_projectInfo.value();
		projectInfo.addToJsonObject(doc, doc.GetAllocator());
		std::string encoded = ot::String::toBase64Url(doc.toJson());
		args << toCommandLine(ArgumentKey::OpenProject) << QString::fromStdString(encoded);
	}

	if (!m_projectVersion.empty())
	{
		args << toCommandLine(ArgumentKey::ProjectVersion) << QString::fromStdString(m_projectVersion);
	}

	if (!m_scriptFile.isEmpty())
	{
		args << toCommandLine(ArgumentKey::ScriptFile) << m_scriptFile;
	}

	return args;
}

QString StartArgumentParser::createFrontendUrlLink() const
{
	QUrlQuery query;

	if (m_debug)
	{
		query.addQueryItem(toUrl(ArgumentKey::Debug), QString());
	}

	if (m_checkGraphics)
	{
		query.addQueryItem(toUrl(ArgumentKey::CheckGraphics), QString());
	}

	if (getLoginDataSet())
	{
		OT_LOG_E("Login data provided in URL");
	}

	if (getLoginGSSSet())
	{
		query.addQueryItem(toUrl(ArgumentKey::LogInGss), QString());
		const auto& loginGSS = m_loginGSS.value();
		loginGSS.addToQuery(query);
	}

	if (getProjectInfoSet())
	{
		const auto& projectInfo = m_projectInfo.value();
		query.addQueryItem(toUrl(ArgumentKey::OpenProject), QString::fromStdString(projectInfo.getProjectName()));

		if (!m_projectVersion.empty())
		{
			query.addQueryItem(toUrl(ArgumentKey::ProjectVersion), QString::fromStdString(m_projectVersion));
		}
	}

	if (!m_scriptFile.isEmpty())
	{
		query.addQueryItem(toUrl(ArgumentKey::ScriptFile), m_scriptFile);
	}

	QUrl url;
	url.setScheme(shareFrontendScheme());
	url.setHost(shareFrontendHost());
	url.setQuery(query);

	return url.toString(QUrl::FullyEncoded);
}

QString StartArgumentParser::createShareLink() const
{
	QString frontendLink = createFrontendUrlLink();

	if (frontendLink.isEmpty())
	{
		OT_LOG_E("No arguments to create share link");
		return QString();
	}

	AppBase* app = AppBase::instance();
	OTAssertNullptr(app);
	
	const auto& loginData = app->getCurrentLoginData();
	const auto& gss = loginData.getGss();
	QString gssUrl = gss.getUrl();
	if (gssUrl.isEmpty())
	{
		OT_LOG_E("No GSS URL provided");
		return QString();
	}
	if (gssUrl == "localhost")
	{
		gssUrl = "127.0.0.1";
	}
	
	QUrl gssEndpoint;
	gssEndpoint.setScheme(shareBackendScheme());
	gssEndpoint.setHost(gssUrl);
	bool ok = false;
	gssEndpoint.setPort(gss.getPort().toInt(&ok));
	if (!ok)
	{
		OT_LOG_E("Invalid GSS port: " + gss.getPort().toStdString());
		return QString();
	}
	gssEndpoint.setPath(shareBackendEndpoint());

	QUrlQuery query;
	query.addQueryItem(shareBackendArgumentsKey(), frontendLink);

	gssEndpoint.setQuery(query);

	return gssEndpoint.toString(QUrl::FullyEncoded);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter

void StartArgumentParser::clear()
{
	m_debug = false;
	m_checkGraphics = false;

	m_loginData = std::nullopt;
	m_loginGSS = std::nullopt;

	m_projectInfo = std::nullopt;
	m_projectVersion.clear();

	m_scriptFile.clear();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Helper

bool StartArgumentParser::parseUrl(const QString& _url)
{
	QString normalizedUrl = _url;

	if (normalizedUrl.startsWith("opentwin:://", Qt::CaseInsensitive))
	{
		normalizedUrl.replace(0, 11, "opentwin://");
	}

	const QUrl url(normalizedUrl);

	if (!url.isValid())
	{
		OT_LOG_E("Invalid OpenTwin URL: " + _url.toStdString());
		return false;
	}

	if (url.scheme() != "opentwin")
	{
		OT_LOG_E("Unsupported URL scheme: " + url.scheme().toStdString());
		return false;
	}

	const QUrlQuery query(url);

	// Debug
	if (query.hasQueryItem(toUrl(ArgumentKey::Debug)))
	{
		m_debug = true;
	}

	// Check graphics
	if (query.hasQueryItem(toUrl(ArgumentKey::CheckGraphics)))
	{
		m_checkGraphics = true;
	}

	// Login
	if (query.hasQueryItem(toUrl(ArgumentKey::LogInData)))
	{
		OT_LOG_E("Login data provided in URL");
		return false;
	}

	// Auto login
	if (query.hasQueryItem(toUrl(ArgumentKey::LogInGss)))
	{
		LogInGSSEntry newEntry;
		auto errorString = newEntry.readFromQuery(query);
		if (errorString.has_value())
		{
			OT_LOG_E("Failed to read auto login data from URL: " + errorString.value().toStdString());
			return false;
		}

		m_loginGSS = newEntry;
	}

	// Open project
	if (query.hasQueryItem(toUrl(ArgumentKey::OpenProject)))
	{
		const QString projectName = query.queryItemValue(toUrl(ArgumentKey::OpenProject));

		ot::ProjectInformation newInfo;
		newInfo.setProjectName(projectName.toStdString());
		m_projectInfo = newInfo;

		// Project version
		if (query.hasQueryItem(toUrl(ArgumentKey::ProjectVersion)))
		{
			m_projectVersion = query.queryItemValue(toUrl(ArgumentKey::ProjectVersion)).toStdString();
		}
	}

	// Script
	if (query.hasQueryItem(toUrl(ArgumentKey::ScriptFile)))
	{
		m_scriptFile = query.queryItemValue(toUrl(ArgumentKey::ScriptFile));
	}

	return true;
}

bool StartArgumentParser::parseCommandLine()
{
	// Prepare parser
	QCommandLineParser parser;
	parser.setApplicationDescription("OpenTwin Frontend");
	parser.addHelpOption();

	// Define command line options
	QCommandLineOption debugOption(toString(ArgumentKey::Debug), "Run test code.");
	parser.addOption(debugOption);

	QCommandLineOption checkGraphicsOption({ toString(ArgumentKey::CheckGraphicsShort), toString(ArgumentKey::CheckGraphics) }, "Check the graphics settings.");
	parser.addOption(checkGraphicsOption);

	QCommandLineOption dataLoginOption(
		toString(ArgumentKey::LogInData),
		"Login with JSON data.",
		"string");
	parser.addOption(dataLoginOption);

	QCommandLineOption gssLoginOption(
		toString(ArgumentKey::LogInGss),
		"Attempt to log in automatically using the initial login data.",
		"string");
	parser.addOption(gssLoginOption);

	QCommandLineOption openProjectOption(
		toString(ArgumentKey::OpenProject),
		"Open project with JSON data.",
		"string");
	parser.addOption(openProjectOption);

	QCommandLineOption projectVersionOption(
		toString(ArgumentKey::ProjectVersion),
		"Specify the project version to open.",
		"string");
	parser.addOption(projectVersionOption);

	QCommandLineOption scriptFileOption(
		toString(ArgumentKey::ScriptFile),
		"Execute the specified script file after initialization.",
		"string");
	parser.addOption(scriptFileOption);

	// Process the actual command line arguments
	parser.process(*QCoreApplication::instance());

	// Check for debug option
	if (parser.isSet(debugOption))
	{
		m_debug = true;
	}

	// Check for graphics check option
	if (parser.isSet(checkGraphicsOption))
	{
		m_checkGraphics = true;
	}

	// Check for auto login option
	if (parser.isSet(gssLoginOption))
	{
		QString encodedData = parser.value(gssLoginOption);
		std::string decoded = ot::String::fromBase64Url(encodedData.toStdString());

		if (decoded.empty())
		{
			OT_LOG_E("Auto login option set but no data provided");
			return false;
		}

		ot::JsonDocument doc;
		if (!doc.fromJson(decoded))
		{
			OT_LOG_E("Failed to parse auto login data from command line argument");
			return false;
		}

		LogInGSSEntry newEntry;
		newEntry.setFromJsonObject(doc.getConstObject());

		m_loginGSS = newEntry;
	}

	// Check for login option
	if (parser.isSet(dataLoginOption))
	{
		const QString loginDataStr = parser.value(dataLoginOption);
		std::string decoded = ot::String::fromBase64Url(loginDataStr.toStdString());
		if (decoded.empty())
		{
			OT_LOG_E("Login option set but no data provided");
			return false;
		}

		ot::JsonDocument doc;
		if (!doc.fromJson(decoded))
		{
			OT_LOG_E("Failed to parse login data from command line argument");
			return false;
		}

		LoginData newLoginData;
		newLoginData.setFromRequiredDataJson(doc.getConstObject());

		m_loginData = newLoginData;
	}

	// Check for open project option
	if (parser.isSet(openProjectOption))
	{
		const QString projectDataStr = parser.value(openProjectOption);
		std::string decoded = ot::String::fromBase64Url(projectDataStr.toStdString());
		if (decoded.empty())
		{
			OT_LOG_E("Open project option set but no data provided");
			return false;
		}
		ot::JsonDocument doc;
		if (!doc.fromJson(decoded))
		{
			OT_LOG_E("Failed to parse project data from command line argument");
			return false;
		}

		ot::ProjectInformation newProjectInfo;
		newProjectInfo.setFromJsonObject(doc.getConstObject());
		m_projectInfo = newProjectInfo;

		// Check for project version option
		if (parser.isSet(projectVersionOption))
		{
			m_projectVersion = parser.value(projectVersionOption).toStdString();
		}
	}

	// Check for script file option
	if (parser.isSet(scriptFileOption))
	{
		m_scriptFile = parser.value(scriptFileOption);
	}

	return true;
}

QString StartArgumentParser::toString(ArgumentKey _key) const
{
	switch (_key)
	{
	case ArgumentKey::Debug: return "debug";
	case ArgumentKey::CheckGraphics: return "checkgraphics";
	case ArgumentKey::CheckGraphicsShort: return "c";
	case ArgumentKey::LogInData: return "login";
	case ArgumentKey::LogInGss: return "autologin";
	case ArgumentKey::OpenProject: return "openproject";
	case ArgumentKey::ProjectVersion: return "projversion";
	case ArgumentKey::ScriptFile: return "script";
	default:
		OT_LOG_ES("Unknown argument key: " + std::to_string(static_cast<int>(_key)));
		return "unknown";
	}
}

QString StartArgumentParser::toUrl(ArgumentKey _key) const
{
	return toString(_key);
}

QString StartArgumentParser::toCommandLine(ArgumentKey _key) const
{
	return "--" + toString(_key);
}
