/*
 *	File:		aWindowEventHandler.h
 *	Package:	akGui
 *
 *  Created on: November 09, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#pragma once

 // AK header
#include <akCore/globalDataTypes.h>

namespace ak {
	class UICORE_API_EXPORT aWindowEventHandler {
	public:
		aWindowEventHandler() {}

		virtual ~aWindowEventHandler() {}

		// ###################################################################################

		// Event handling routines

		//! @brief This function will be called by the window when the closeEvent occurs
		//! If the return value is true the closeEvent will be acceptet otherwise ignored 
		virtual bool closeEvent(void) { return true; }
	};
}