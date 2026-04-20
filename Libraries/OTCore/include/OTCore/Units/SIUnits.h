//@ otlicense
#pragma once
#include <string>
#include "OTCore/Units/UnitSystem.h"
#include "OTCore/CoreAPIExport.h"
namespace ot
{
	class OT_CORE_API_EXPORT SIUnits : public ot::UnitSystem
	{
	public:
		static SIUnits& instance();
		std::string getSystemName() override { return "SI"; }
        
		static std::string getAngleUnitDegrees() { return "deg"; }
		static std::string getAngleUnitRad() { return "rad"; }
		static std::string getUnitDecibel() { return "dB"; }
    private:
		SIUnits();
    };

};
