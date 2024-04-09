/*
 *	File:		aApplication.h
 *	Package:	akGui
 *
 *  Created on: October 23, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#pragma once

// AK header
#include <akCore/globalDataTypes.h>
#include "OTCore/OTClassHelper.h"
// Qt header
#include <qapplication.h>				// base class

namespace ak {

	class UICORE_API_EXPORT aApplication : public QApplication {
		Q_OBJECT
		OT_DECL_NOCOPY(aApplication)
	public:
		aApplication();
		virtual ~aApplication();
	};
}