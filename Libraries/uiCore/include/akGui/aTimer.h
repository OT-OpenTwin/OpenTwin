// @otlicense

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