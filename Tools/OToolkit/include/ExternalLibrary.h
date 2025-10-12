//! @file ExternalLibrary.h
//! @author Alexander Kuester (alexk95)
//! @date October 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OToolkit API header
#include "OToolkitAPI/Tool.h"

// OpenTwin header
#include "OTSystem/SystemProcess.h"
#include "OTCore/OTClassHelper.h"

// std header
#include <list>

class ExternalLibrary {
	OT_DECL_NOCOPY(ExternalLibrary)
public:
	ExternalLibrary();
	virtual ~ExternalLibrary();

	bool load(const QString& _path);

	const std::list<otoolkit::Tool*> getTools(void) const { return m_tools; };

private:
	ot::ExternalLibraryInstance_t m_instance;
	std::list<otoolkit::Tool*> m_tools;
};