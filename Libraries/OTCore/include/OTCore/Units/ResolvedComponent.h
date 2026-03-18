#pragma once
#include "OTCore/Units/Descriptors.h"
namespace ot
{
    struct ResolvedComponent {
            PrefixDescriptor prefix;   // factor=1, symbol="" if no prefix
            UnitDescriptor   base;
 
        };
}
