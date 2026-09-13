#pragma once
// @otlicense
#include <string>
#include <list>

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

	private:
		std::string m_tagStringRaw = "RAW";
		std::string m_tagStringRefined = "REFINED";
		std::string m_tagStringCompliant = "COMPLIANT";
	};
}

