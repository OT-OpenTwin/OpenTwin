#include "OTCore/Units/UnitRegistry.h"
#include <cassert>
#include <algorithm>
using namespace ot;


void UnitRegistry::addUnit(UnitDescriptor&& _ud)
{
    assert(!_ud.m_symbol.empty());
    m_unitsBySymbol[_ud.m_symbol] = std::move(_ud);
}

void ot::UnitRegistry::addPrefix(PrefixDescriptor&& _pd) 
{
    assert(!_pd.m_symbol.empty());
    m_prefixes.push_back(std::move(_pd));
    // keep sorted longest-first for greedy prefix matching
    std::sort(m_prefixes.begin(), m_prefixes.end(),
        [](const PrefixDescriptor& _a, const PrefixDescriptor& _b) {
            return _a.m_symbol.size() > _b.m_symbol.size();
        });
}

std::optional<UnitDescriptor> ot::UnitRegistry::findUnit(const std::string& _symbol) const
{
    auto unitBySymbol = m_unitsBySymbol.find(_symbol);
    if (unitBySymbol == m_unitsBySymbol.end())
    {
        return std::nullopt;
    }
    else
    {
        return unitBySymbol->second;
    }
}

std::optional<PrefixDescriptor> ot::UnitRegistry::findPrefix(const std::string& _symbol) const
{
    for (const auto& prefix : m_prefixes)
    {
        if (prefix.m_symbol == _symbol)
        {
            return prefix;
        }
    }
    return std::nullopt;
}