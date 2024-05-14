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

	//! \class OTObjectBase
	//! \brief This is a empty base class without any functionality to be used as generalization base class
	class OT_CORE_API_EXPORT OTObjectBase {
	public:
		OTObjectBase();
		virtual ~OTObjectBase();
	};

}