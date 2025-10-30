// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"

// Qt header
#include <QtCore/qstring.h>

// std header
#include <list>

class ToolManager;
class ExternalLibrary;

class ExternalLibraryManager {
	OT_DECL_NOCOPY(ExternalLibraryManager)
public:
	ExternalLibraryManager();
	virtual ~ExternalLibraryManager();

	void importTools(ToolManager* _toolManager);

private:
	std::list<ExternalLibrary*> m_externalLibs;
};