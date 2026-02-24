//@ otlicense
#pragma once
#include <string>

namespace ot
{
	namespace SIUnits
	{
		namespace Base
		{
			static const std::string getMeterUnit() { return "m"; }
			static const std::string getKilogramUnit() { return "kg"; }
			static const std::string getSecondUnit() { return "s"; }
			static const std::string getAmpereUnit() { return "A"; }
			static const std::string getKelvinUnit() { return "K"; }
			static const std::string getMoleUnit() { return "mol"; }
			static const std::string getCandelaUnit() { return "cd"; }
		}
		namespace Derived
		{
			static const std::string getRadianUnit() { return "rad"; }
			static const std::string getDegreeUnit() { return "°"; } //Note: The degree symbol is not an official SI unit, but it is commonly used for angles.
			static const std::string getSteradianUnit() { return "sr"; }
			static const std::string getHertzUnit() { return "Hz"; }
			static const std::string getNewtonUnit() { return "N"; }
			static const std::string getPascalUnit() { return "Pa"; }
			static const std::string getJouleUnit() { return "J"; }
			static const std::string getWattUnit() { return "W"; }
			static const std::string getCoulombUnit() { return "C"; }
			static const std::string getVoltUnit() { return "V"; }
			static const std::string getFaradUnit() { return "F"; }
			static const std::string getOhmUnit() { return "Ohm"; }
			static const std::string getSiemensUnit() { return "S"; }
			static const std::string getWeberUnit() { return "Wb"; }
			static const std::string getTeslaUnit() { return "T"; }
			static const std::string getHenryUnit() { return "H"; }
			static const std::string getLumenUnit() { return "lm"; }
			static const std::string getLuxUnit() { return "lx"; }
			static const std::string getBecquerelUnit() { return "Bq"; }
		}

		namespace Prefix
		{
			static const std::string getKiloPrefix() { return "k"; }
			static const std::string getMegaPrefix() { return "M"; }
			static const std::string getGigaPrefix() { return "G"; }
			static const std::string getTeraPrefix() { return "T"; }
			static const std::string getPetaPrefix() { return "P"; }
			static const std::string getExaPrefix() { return "E"; }
			static const std::string getZettaPrefix() { return "Z"; }
			static const std::string getYottaPrefix() { return "Y"; }
			static const std::string getMilliPrefix() { return "m"; }
			static const std::string getMicroPrefix() { return "µ"; }
			static const std::string getNanoPrefix() { return "n"; }
			static const std::string getPicoPrefix() { return "p"; }
			static const std::string getFemtoPrefix() { return "f"; }
			static const std::string getAttoPrefix() { return "a"; }
			static const std::string getZeptoPrefix() { return "z"; }
			static const std::string getYoctoPrefix() { return "y"; }
		}
	}
};
