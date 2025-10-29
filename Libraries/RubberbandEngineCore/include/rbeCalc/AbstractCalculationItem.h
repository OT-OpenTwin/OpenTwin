// @otlicense

#pragma once

// RBE header
#include <rbeCore/dataTypes.h>

namespace rbeCalc {

	class RBE_API_EXPORT AbstractCalculationItem {
	public:
		AbstractCalculationItem() {}
		virtual ~AbstractCalculationItem() {}
		
		virtual coordinate_t value(void) const = 0;

	private:

		AbstractCalculationItem(AbstractCalculationItem&) = delete;
		AbstractCalculationItem& operator = (AbstractCalculationItem&) = delete;
	};

}