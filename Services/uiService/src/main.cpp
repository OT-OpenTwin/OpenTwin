/*
 * dllmain.h
 *
 *	Author: Alexander Kuester
 *  Copyright (c) 2020 openTwin
 */

// App header
#include "AppBase.h"
#include "ExternalServicesComponent.h"

// OT header
#include "OTCore/Logger.h"
#include "OTWidgets/IconManager.h"
#include "OTWidgets/GlobalColorStyle.h"
#include "OTWidgets/WidgetViewManager.h"
#include "OTWidgets/GraphicsItemLoader.h"
#include "OTWidgets/ApplicationPropertiesManager.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/ServiceLogNotifier.h"

// uiCore
#include "akAPI/uiAPI.h"

// Qt header
#include <QtCore/qdir.h>
#include <QtGui/qopenglcontext.h>
#include <QtGui/qopenglfunctions.h>
#include <QtGui/qoffscreensurface.h>
#include <QtWidgets/qmessagebox.h>
#include <QtWidgets/qapplication.h>
#include <QtCore/qcommandlineparser.h>

// C++ header
#include <exception>

void initializeLogging(void) {
	// Get logging URL
	QByteArray loggingenv = qgetenv("OPEN_TWIN_LOGGING_URL");

	// Initialize logging
	#ifdef OT_RELEASE_DEBUG
		ot::ServiceLogNotifier::initialize(OT_INFO_SERVICE_TYPE_UI, loggingenv.toStdString(), true);
	#else
		ot::ServiceLogNotifier::initialize(OT_INFO_SERVICE_TYPE_UI, loggingenv.toStdString(), false);
	#endif
}

QApplication* initializeQt(int &_argc, char* _argv[]) {
	QApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
	QApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

	QLocale::setDefault(QLocale(QLocale::English));

	QApplication* newApp = new QApplication(_argc, _argv);
	ak::uiAPI::ini(newApp, "OpenTwin", "UserFrontend");

	ot::GlobalColorStyle::instance().setApplication(newApp);

	return newApp;
}

void initializeAppBase(void) {
	AppBase* app = AppBase::instance();
	app->setSiteID(0);
	app->getExternalServicesComponent()->setRelayServiceIsRequired();
}

bool initializeOpenGL(bool checkGraphics) {
	// Check for a sufficient OpenGL version
	QOffscreenSurface surf;
	surf.create();

	QOpenGLContext ctx;
	ctx.create();
	ctx.makeCurrent(&surf);

	std::string version = (const char*)ctx.functions()->glGetString(GL_VERSION);
	int major = std::atoi(version.c_str());
	if (major < 2)
	{
		std::string errorString = "The system supports OpenGL version " + version + " only.\n"
			"At least OpenGL version 2.0.0 is required.\n\n"
			"As a fallback solution, slower software rendering will now be activated.\n\n"
			"Please restart the application for the changes to take effect.";

		std::string installPath = QCoreApplication::applicationDirPath().toLocal8Bit().constData();
		std::string sourceFile  = installPath + "/opengl32sw.dll";
		std::string destFile    = installPath + "/opengl32.dll";

		if (!std::filesystem::exists(sourceFile)) 
		{
			std::string dev_root = qgetenv("OPENTWIN_DEV_ROOT").constData();

			if (dev_root.empty())
			{
				errorString = "The system supports OpenGL version " + version + " only.\n"
						   	  "At least OpenGL version 2.0.0 is required.";
			}
			else
			{
				sourceFile = dev_root + "/Deployment/opengl32sw.dll";
				destFile = dev_root + "/Deployment/opengl32.dll";

				std::filesystem::copy_file(sourceFile, destFile);
			}
		}
		else
		{
			std::filesystem::copy_file(sourceFile, destFile);
		}

		//OT_LOG_E(errorString);  This does not worl if OpenGL is not properly initialized due to the HTML styling

		if (!checkGraphics)
		{
			QMessageBox msgBox(QMessageBox::Critical, "OpenGL error", QString::fromStdString(errorString), QMessageBox::Ok);
			msgBox.exec();
		}
		return false;
	}
	else {
		return true;
	}
}

bool initializeAssets(void) {
	int iconPathCounter = 0;
	int stylePathCounter = 0;
	int graphicsPathCounter = 0;

#ifdef _DEBUG
	if (ot::IconManager::addSearchPath(QString(qgetenv("OPENTWIN_DEV_ROOT") + "/Assets/Icons/"))) {
		iconPathCounter++;
	}
	if (ot::GlobalColorStyle::instance().addStyleRootSearchPath(QString(qgetenv("OPENTWIN_DEV_ROOT") + "/Assets/ColorStyles/"))) {
		stylePathCounter++;
	}
	if (ot::GraphicsItemLoader::instance().addSearchPath(QString(qgetenv("OPENTWIN_DEV_ROOT") + "/Assets/GraphicsItems/"))) {
		graphicsPathCounter++;
	}
#else
	if (ot::IconManager::addSearchPath(QDir::currentPath() + "/icons/")) {
		iconPathCounter++;
	}
	if (ot::GlobalColorStyle::instance().addStyleRootSearchPath(QDir::currentPath() + "/ColorStyles/")) {
		stylePathCounter++;
	}
	if (ot::GraphicsItemLoader::instance().addSearchPath(QDir::currentPath() + "/GraphicsItems/")) {
		graphicsPathCounter++;
	}
#endif // _DEBUG

	// Check if at least one icon directory was found
	if (iconPathCounter == 0) {
		OTAssert(0, "No icon path was found!");
		OT_LOG_E("No icon path found");
		QMessageBox msgBox(QMessageBox::Critical, "Assets error", "No icon path was found. Try to reinstall the application", QMessageBox::Ok);
		msgBox.exec();
		return false;
	}

	// Check if at least one style directory was found
	if (stylePathCounter == 0) {
		OT_LOG_EA("No color style path found");
		QMessageBox msgBox(QMessageBox::Critical, "Assets error", "No color style path was found. Try to reinstall the application", QMessageBox::Ok);
		msgBox.exec();
		return false;
	}

	// Check if at least one graphics item directory was found
	if (graphicsPathCounter == 0) {
		OT_LOG_EA("No graphics item path found");
		QMessageBox msgBox(QMessageBox::Critical, "Assets error", "No graphics item path was found. Try to reinstall the application", QMessageBox::Ok);
		msgBox.exec();
		return false;
	}

	// Initialize application icon
	ot::IconManager::setApplicationIcon(ot::IconManager::getIcon("Application/OpenTwin.ico"));

	// Initialize color style
	ot::GlobalColorStyle::instance().scanForStyleFiles();

	if (ot::GlobalColorStyle::instance().hasStyle(ot::toString(ot::ColorStyleName::BrightStyle))) {
		ot::GlobalColorStyle::instance().setCurrentStyle(ot::toString(ot::ColorStyleName::BrightStyle));
	}
	else {
		OT_LOG_EA("Bright style not found");
		QMessageBox msgBox(QMessageBox::Critical, "Assets error", "Default " + QString::fromStdString(ot::toString(ot::ColorStyleName::BrightStyle)) + " color style is missing. Try to reinstall the application", QMessageBox::Ok);
		msgBox.exec();
		return false;
	}

	return true;
}

bool initializeComponents(void) {
	// Initialize Application Settings
	ot::ApplicationPropertiesManager::instance().setReplaceExistingPropertiesOnMerge(true);

	// Initialize Widget view manager
	ot::WidgetViewManager::instance().initialize();

	return true;
}

int main(int _argc, char *_argv[])
{
	try {
		// Initialize

		initializeLogging();

		QApplication* app = initializeQt(_argc, _argv);

		QCommandLineParser parser;
		QCommandLineOption checkGraphicsOption("c");
		parser.addOption(checkGraphicsOption);

		parser.process(*app);
		bool checkGraphics = parser.isSet(checkGraphicsOption);

		initializeAppBase();

		if (!initializeOpenGL(checkGraphics)) {
			return -1;
		}

		if (checkGraphics)
		{
			return 0;
		}

		if (!initializeAssets()) {
			return -2;
		}

		if (!initializeComponents()) {
			return -3;
		}

		if (!AppBase::instance()->initialize()) {
			return -4;
		}

		// LogIn
		if (!AppBase::instance()->logIn()) {
			return 0;
		}

		// Run Main Event Loop
		int status = app->exec();

		// Cleanup
		ExternalServicesComponent * extComp = AppBase::instance()->getExternalServicesComponent();
		if (extComp != nullptr) {
			extComp->shutdown(); 
		}

		return status;
	}
	catch (const std::exception & e) {
		std::string errorText = e.what();
		assert(0); // Something went wrong
		return -601;
	}
	catch (...) {
		assert(0);	// Something went horribly wrong
		return -602;
	}
}