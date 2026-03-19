#pragma once
#include <string>
#include "OTCore/Units/Dimension.h"
#include "UnitKind.h"
namespace ot
{
    struct UnitDescriptor
    {
        std::string m_symbol;
        std::string m_name;           // human-readable, e.g. "kilometer"
        std::string m_unitSet;        // "SI", "imperial", "CGS", ...
        Dimension   m_dimension;
        double      toSIScale = 1.0; // value * toSIScale + toSIOffset = SI value
        double      toSIOffset = 0.0; // non-zero only for affine units (°C, °F)
        UnitKind m_unitKind = UnitKind::Linear;
        
        bool        isAffine() const { return toSIOffset != 0.0; } // Affine units Affine units are measurement units that require both a scaling factor(multiplication) and an offset(addition) to convert to a base unit, unlike linear units which only require scaling.Common examples are temperature scales like Celsius and Fahrenheit
        bool isLog() const { return m_unitKind == UnitKind::LogPower || m_unitKind == UnitKind::LogAmplitude; }
        double getLogMultiplier() const { return (m_unitKind == UnitKind::LogAmplitude) ? 20. : 10.; }
    };  

    struct PrefixDescriptor
    {
        std::string m_symbol;
        std::string m_name;
        double m_factor; 
    };
}
