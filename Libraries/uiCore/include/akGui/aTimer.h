/*
 *	File:		aTimer.h
 *	Package:	akGui
 *
 *  Created on: September 25, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#pragma once

// Qt header
#include <qtimer.h>					// Base class

// AK header
#include <akCore/globalDataTypes.h>		// API Export
#include <akCore/aObject.h>

namespace ak {
	class UICORE_API_EXPORT aTimer : public QTimer, public aObject {
	public:
		aTimer();
		virtual ~aTimer();
	};
}