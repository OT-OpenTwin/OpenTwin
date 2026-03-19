#include "OTCore/Units/UnitSystem.h"

using namespace ot;
std::optional<ResolvedComponent> UnitSystem::resolve(const std::string& _symbol) const
        {
            // 1. Try full symbol as a registered unit first (e.g. "km" if registered)
            if (auto unit = m_registry.findUnit(_symbol))
            {
                return ResolvedComponent{ {"", "identity", 1.0}, *unit };
            }
            // 2. Greedy longest-prefix match
            for (const auto& pfx : m_registry.getPrefixes()) {
                if (_symbol.size() > pfx.m_symbol.size() &&
                    _symbol.substr(0, pfx.m_symbol.size()) == pfx.m_symbol)
                {
                    auto rest = _symbol.substr(pfx.m_symbol.size());
                    if (auto unit = m_registry.findUnit(rest))
                    {
                        return ResolvedComponent{ pfx, *unit };
                    }
                }
            }
            return std::nullopt;
        }