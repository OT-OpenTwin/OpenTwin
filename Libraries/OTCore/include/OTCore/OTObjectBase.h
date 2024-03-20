//! @file OTObjectBase.h
//!
//! Empty base class for OpenTwin objects
//! 
//! @author Alexander Kuester (alexk95)
//! @date November 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/CoreAPIExport.h"

namespace ot {

	class OT_CORE_API_EXPORT OTObjectBase {
	public:
		OTObjectBase();
		virtual ~OTObjectBase();
	};

}