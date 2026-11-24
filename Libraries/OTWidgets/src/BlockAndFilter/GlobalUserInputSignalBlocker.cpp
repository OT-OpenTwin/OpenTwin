// @otlicense

// OpenTwin header
#include "OTWidgets/BlockAndFilter/GlobalUserInputSignalBlocker.h"

// Qt header
#include <QtWidgets/qapplication.h>

ot::GlobalUserInputSignalBlocker& ot::GlobalUserInputSignalBlocker::instance()
{
	static GlobalUserInputSignalBlocker g_instance;
	return g_instance;
}

void ot::GlobalUserInputSignalBlocker::install()
{
	auto& obj = GlobalUserInputSignalBlocker::instance();
	OTAssertNullptr(obj.m_blocker);
	if (!obj.m_installed)
	{
		auto* app = QApplication::instance();
		if (!app)
		{
			throw Exception::ObjectNotFound("QApplication instance not found. Please ensure that a QApplication is created before installing the GlobalUserInputSignalBlocker.");
		}
		if (!obj.m_wasInstalled)
		{
			obj.m_blocker->setParent(app);
			obj.m_wasInstalled = true;
		}
		app->installEventFilter(obj.m_blocker);
		obj.m_installed = true;
	}
}

void ot::GlobalUserInputSignalBlocker::uninstall()
{
	auto& obj = GlobalUserInputSignalBlocker::instance();
	OTAssertNullptr(obj.m_blocker);
	if (obj.m_installed)
	{
		auto* app = QApplication::instance();
		if (!app)
		{
			throw Exception::ObjectNotFound("QApplication instance not found. Please ensure that a QApplication is created before uninstalling the GlobalUserInputSignalBlocker.");
		}
		app->removeEventFilter(obj.m_blocker);
		obj.m_installed = false;
	}
}

ot::GlobalUserInputSignalBlocker::GlobalUserInputSignalBlocker()
	: m_installed(false), m_wasInstalled(false), m_blocker(nullptr)
{
	m_blocker = new UserInputSignalBlocker;
}

ot::GlobalUserInputSignalBlocker::~GlobalUserInputSignalBlocker()
{
	if (!m_wasInstalled)
	{
		OTAssertNullptr(m_blocker);
		delete m_blocker;
	}
	// Else: Managed by Qt, no need to delete m_blocker
}
