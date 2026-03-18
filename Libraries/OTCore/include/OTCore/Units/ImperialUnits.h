#pragma once
#include "OTCore/Units/UnitSystem.h"
#include "OTCore/CoreAPIExport.h"

namespace ot
{
	class OT_CORE_API_EXPORT ImperialUnits : public UnitSystem
	{
	public:
		static ImperialUnits& instance();
		std::string getSystemName() override { return "imperial"; }

	private:
		ImperialUnits();
	};
}
