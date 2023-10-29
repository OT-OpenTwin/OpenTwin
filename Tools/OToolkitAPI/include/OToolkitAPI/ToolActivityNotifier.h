//! @file ToolActivityNotifier.h
//! @author Alexander Kuester (alexk95)
//! @date October 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

#include "OToolkitAPI/Tool.h"

namespace otoolkit {

	class ToolActivityNotifier {
	public:
		ToolActivityNotifier() {};
		virtual ~ToolActivityNotifier() {};

		virtual void toolStarted(otoolkit::Tool* _tool) = 0;
		virtual void toolStopped(otoolkit::Tool* _tool) = 0;

		virtual void toolWasShown(otoolkit::Tool* _tool) = 0;
		virtual void toolWasHidden(otoolkit::Tool* _tool) = 0;
	};

}
