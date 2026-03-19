#pragma once
#include "OTCore/Units/Descriptors.h"
namespace ot
{
    struct ResolvedComponent {
            PrefixDescriptor m_prefix;   // factor=1, symbol="" if no prefix
            UnitDescriptor   m_base;
            double   totalScale() const 
            {
                return m_prefix.m_factor * m_base.toSIScale;
            }
        };
}
