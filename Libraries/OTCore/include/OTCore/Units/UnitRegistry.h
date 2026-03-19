#pragma once
#include <string>
#include <optional>
#include <map>
#include <vector>
#include "OTCore/Units/Descriptors.h"
#include "OTCore/Units/ResolvedComponent.h"

namespace ot
{
    class UnitRegistry
    {
    public:
        void addUnit(UnitDescriptor&& _unitDescriptor);
        void addPrefix(PrefixDescriptor&& _prefixDescriptor);
        std::optional<UnitDescriptor> findUnit(const std::string& _symbol) const;
        std::optional<PrefixDescriptor> findPrefix(const std::string& _symbol) const;
        const std::vector<PrefixDescriptor>& getPrefixes() const { return m_prefixes; }
        void addAlias(const std::string& alias, const std::string& canonicalSym);
    private:
        std::map<std::string, UnitDescriptor> m_unitsBySymbol;
        std::vector<PrefixDescriptor> m_prefixes;
    };
}

