#pragma once
#include <string>
#include "OTCore/Units/Dimension.h"
namespace ot
{
    struct UnitDescriptor
    {
        std::string m_symbol;
        std::string m_name;           // human-readable, e.g. "kilometre"
        std::string m_unitSet;        // "SI", "imperial", "CGS", ...
        Dimension   m_dimension;
        double      toSIScale = 1.0; // value * toSIScale + toSIOffset = SI value
        double      toSIOffset = 0.0; // non-zero only for affine units (°C, °F)
        bool        isAffine() const { return toSIOffset != 0.0; }
    };

    struct PrefixDescriptor
    {
        std::string m_symbol;
        std::string m_name;
        double m_factor; 
    };
}
