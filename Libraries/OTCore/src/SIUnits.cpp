#include "OTCore/Units/SIUnits.h"

using namespace ot;

SIUnits& SIUnits::instance()
{
	static SIUnits INSTANCE;
	return INSTANCE;
}


SIUnits::SIUnits()
{
	//Base units
    for (auto& [sym, name, fac] : std::initializer_list<std::tuple<const char*, const char*, double>>{
        {"Y",  "yotta", 1e24},
        {"Z",  "zetta", 1e21},
        {"E",  "exa",   1e18},
        {"P",  "peta",  1e15},
        {"T",  "tera",  1e12},
        {"G",  "giga",  1e9 },
        {"M",  "mega",  1e6 },
        {"k",  "kilo",  1e3 },
        {"h",  "hecto", 1e2 },
        {"da", "deca",  1e1 },
        {"d",  "deci",  1e-1},
        {"c",  "centi", 1e-2},
        {"m",  "milli", 1e-3},
        {"µ",  "micro", 1e-6},
        {"u",  "micro", 1e-6},
        {"n",  "nano",  1e-9},
        {"p",  "pico",  1e-12},
        {"f",  "femto", 1e-15},
        {"a",  "atto",  1e-18},
        {"z",  "zepto", 1e-21},
        {"y",  "yocto", 1e-24},
        })
    {
        m_registry.addPrefix({ sym, name, fac });
    }

    auto helper = [](std::string _sym, std::string _name, std::string _set, Dimension _dim, double _scale = 1., double _offset = 0.) {
            return UnitDescriptor{ _sym, _name, _set, _dim, _scale, _offset };
        };

    // --- SI Base Units ---
    m_registry.addUnit(helper("m", "metre", "SI", LEN));
    m_registry.addUnit(helper("g", "gram", "SI", MASS, 1e-3));  // base is kg
    m_registry.addUnit(helper("s", "second", "SI", TIME));
    m_registry.addUnit(helper("A", "ampere", "SI", CURR));
    m_registry.addUnit(helper("K", "kelvin", "SI", TEMP));
    m_registry.addUnit(helper("mol", "mole", "SI", AMOUNT));
    m_registry.addUnit(helper("cd", "candela", "SI", LUM));

    // --- SI Derived Units ---
    m_registry.addUnit(helper("N", "newton", "SI", FORCE));
    m_registry.addUnit(helper("J", "joule", "SI", ENERGY));
    m_registry.addUnit(helper("W", "watt", "SI", POWER));
    m_registry.addUnit(helper("Pa", "pascal", "SI", PRESS));
    m_registry.addUnit(helper("Hz", "hertz", "SI", { 0,0,-1,0,0,0,0 }));
    m_registry.addUnit(helper("V", "volt", "SI", { 1,2,-3,-1,0,0,0 }));
    m_registry.addUnit(helper("C", "coulomb", "SI", { 0,0,1,1,0,0,0 }));
    m_registry.addUnit(helper("F", "farad", "SI", { -1,-2,4,2,0,0,0 }));
    m_registry.addUnit(helper("Ohm", "ohm", "SI", { 1,2,-3,-2,0,0,0 }));
    m_registry.addUnit(helper("T", "tesla", "SI", { 1,0,-2,-1,0,0,0 }));
    m_registry.addUnit(helper("lm", "lumen", "SI", LUM));
    m_registry.addUnit(helper("lx", "lux", "SI", { 0,-2,0,0,0,0,1 }));

    // --- Affine Temperature (SI-adjacent) ---
    m_registry.addUnit(helper("degC", "celsius", "SI", TEMP, 1.0, 273.15));
}

