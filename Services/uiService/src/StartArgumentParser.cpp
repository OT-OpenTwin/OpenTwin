// @otlicense

// Frontend header
#include "StartArgumentParser.h"

// OpenTwin header
#include "OTCore/String.h"
#include "OTCore/LogDispatcher.h"

// Qt header
#include <QtCore/qcommandlineparser.h>

bool StartArgumentParser::parse() {
	// Reset data
	m_debug = false;
	m_logIn = false;
	m_openProject = false;

	m_loginData = LoginData();
	m_projectInfo = ot::ProjectInformation();
	m_projectVersion.clear();

	// Prepare parser
	QCommandLineParser parser;
	parser.setApplicationDescription("OpenTwin Frontend");
	parser.addHelpOption();

	// Define command line options
	QCommandLineOption debugOption("debug", "Run test code.");
	parser.addOption(debugOption);

	QCommandLineOption checkGraphicsOption({ "c", "checkgraphics"}, "Check the graphics settings.");
	parser.addOption(checkGraphicsOption);

	QCommandLineOption loginOption("login",
		"Login with JSON data.",
		"string");
	parser.addOption(loginOption);

	QCommandLineOption openProjectOption("openproject",
		"Open project with JSON data.",
		"string");
	parser.addOption(openProjectOption);

	QCommandLineOption projectVersionOption("projversion",
		"Specify the project version to open.",
		"string");
	parser.addOption(projectVersionOption);

	// Process the actual command line arguments
	parser.process(*QCoreApplication::instance());

	// Check for debug option
	if (parser.isSet(debugOption)) {
		m_debug = true;
	}

	// Check for graphics check option
	if (parser.isSet(checkGraphicsOption)) {
		m_checkGraphics = true;
	}

	// Check for login option
	if (parser.isSet(loginOption)) {
		const QString loginDataStr = parser.value(loginOption);
		std::string decoded = ot::String::fromBase64Url(loginDataStr.toStdString());
		if (decoded.empty()) {
			OT_LOG_E("Login option set but no data provided");
			return false;
		}

		ot::JsonDocument doc;
		if (!doc.fromJson(decoded)) {
			OT_LOG_E("Failed to parse login data from command line argument");
			return false;
		}

		m_loginData.setFromRequiredDataJson(doc.getConstObject());
		m_logIn = true;
	}

	// Check for open project option
	if (parser.isSet(openProjectOption)) {
		const QString projectDataStr = parser.value(openProjectOption);
		std::string decoded = ot::String::fromBase64Url(projectDataStr.toStdString());
		if (decoded.empty()) {
			OT_LOG_E("Open project option set but no data provided");
			return false;
		}
		ot::JsonDocument doc;
		if (!doc.fromJson(decoded)) {
			OT_LOG_E("Failed to parse project data from command line argument");
			return false;
		}
		m_projectInfo.setFromJsonObject(doc.getConstObject());
		m_openProject = true;

		// Check for project version option
		if (parser.isSet(projectVersionOption)) {
			m_projectVersion = parser.value(projectVersionOption).toStdString();
		}
	}

    return true;
}

QStringList StartArgumentParser::createCommandLineArgs() const {
	QStringList args;

	if (m_checkGraphics) {
		args << "--debug";
	}

	if (m_checkGraphics) {
		args << "--checkgraphics";
	}

	if (m_logIn) {
		ot::JsonDocument doc;
		m_loginData.addRequiredDataToJson(doc, doc.GetAllocator());
		std::string encoded = ot::String::toBase64Url(doc.toJson());
		args << "--login" << QString::fromStdString(encoded);
	}
	
	if (m_openProject) {
		ot::JsonDocument doc;
		m_projectInfo.addToJsonObject(doc, doc.GetAllocator());
		std::string encoded = ot::String::toBase64Url(doc.toJson());
		args << "--openproject" << QString::fromStdString(encoded);
	}

	if (!m_projectVersion.empty()) {
		args << "--projversion" << QString::fromStdString(m_projectVersion);
	}

	return args;
}
