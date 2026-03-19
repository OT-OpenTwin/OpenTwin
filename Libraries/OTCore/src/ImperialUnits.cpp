#include "OTCore/Units/ImperialUnits.h"

using namespace ot;

ImperialUnits& ImperialUnits::instance()
{
    static ImperialUnits INSTANCE;
    return INSTANCE;
}

ot::ImperialUnits::ImperialUnits()
{
    auto helper = [](std::string _sym, std::string _name, std::string _set, Dimension _dim, double _scale = 1., double _offset = 0.) {
        return UnitDescriptor{ _sym, _name, _set, _dim, _scale, _offset };
        };
    m_registry.addUnit(helper("degF", "fahrenheit", "imperial", TEMP, 5.0 / 9.0, 459.67));
    m_registry.addUnit(helper("ft", "foot", "imperial", LEN, 0.3048));
    m_registry.addUnit(helper("in", "inch", "imperial", LEN, 0.0254));
    m_registry.addUnit(helper("yd", "yard", "imperial", LEN, 0.9144));
    m_registry.addUnit(helper("mi", "mile", "imperial", LEN, 1609.344));
    m_registry.addUnit(helper("lb", "pound", "imperial", MASS, 0.453592));
    m_registry.addUnit(helper("oz", "ounce", "imperial", MASS, 0.028350));
    m_registry.addUnit(helper("BTU", "BTU", "imperial", ENERGY, 1055.06));
    m_registry.addUnit(helper("psi", "psi", "imperial", PRESS, 6894.76));
    m_registry.addUnit(helper("hp", "horsepower", "imperial", POWER, 745.7));

    m_registry.addAlias(u8"°F", "degF");
}
