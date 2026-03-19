#pragma once
#include "OTCore/Units/UnitRegistry.h"
#include <optional>
#include <string>
#include "OTCore/Units/ResolvedComponent.h"
#include "OTCore/CoreAPIExport.h"
#pragma warning(disable:4251)


namespace ot
{
	class OT_CORE_API_EXPORT UnitSystem
	{
	public:
		virtual std::string getSystemName() { return ""; }
        virtual std::optional<ResolvedComponent> resolve(const std::string& _symbol) const;

        // Dimension shorthands for the base quantities
        static constexpr Dimension DIMLESS{};
        static constexpr Dimension LEN{ 0,1,0,0,0,0,0 };
        static constexpr Dimension MASS{ 1,0,0,0,0,0,0 };
        static constexpr Dimension TIME{ 0,0,1,0,0,0,0 };
        static constexpr Dimension CURR{ 0,0,0,1,0,0,0 };
        static constexpr Dimension TEMP{ 0,0,0,0,1,0,0 };
        static constexpr Dimension AMOUNT{ 0,0,0,0,0,1,0 };
        static constexpr Dimension LUM{ 0,0,0,0,0,0,1 };

        // Dimension shorthands for derived quantities
        static constexpr Dimension FORCE = { 1,1,-2,0,0,0,0 };
        static constexpr Dimension ENERGY = { 1,2,-2,0,0,0,0 };
        static constexpr Dimension POWER = { 1,2,-3,0,0,0,0 };
        static constexpr Dimension PRESS = { 1,-1,-2,0,0,0,0 };
        static constexpr Dimension IRRAD = { 1,0,-3,0,0,0,0 }; // W/m² = kg/s³


	protected:
		UnitRegistry m_registry;
	};

}
