#pragma once
// @otlicense
#include <string>
#include <list>
#include <optional>
#include "OTCore/CoreAPIExport.h"
namespace ot
{
	enum class ZoneTag
	{
		RAW,
		REFINED,
		COMPLIANT
	};

	class OT_CORE_API_EXPORT ZoneTagConverter
	{
	public:
		std::string toString(const ZoneTag& _zoneIndicator) const;
		ZoneTag toZoneTag(const std::string& _zoneTagString) const;
		std::list<std::string> getAllZoneTagStrings() { return { m_tagStringRaw , m_tagStringRefined, m_tagStringCompliant}; }
		std::list<ZoneTag> getAllZoneTags() { return { ZoneTag::RAW, ZoneTag:: REFINED, ZoneTag::COMPLIANT}; }
		std::optional<ZoneTag> nextInLine(ZoneTag _targetZoneTag) 
		{ 
			if (_targetZoneTag == ZoneTag::RAW)
			{
				return ZoneTag::REFINED;
			}
			else if (_targetZoneTag == ZoneTag::REFINED)
			{
				return ZoneTag::COMPLIANT;
			}
			else
			{
				return std::nullopt;
			}
		}
	private:
		std::string m_tagStringRaw = "RAW";
		std::string m_tagStringRefined = "REFINED";
		std::string m_tagStringCompliant = "COMPLIANT";
	};
}

