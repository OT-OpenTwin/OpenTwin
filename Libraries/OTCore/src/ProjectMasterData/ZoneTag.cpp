#include "OTCore/ProductMasterData/ZoneTag.h"

std::string ot::ZoneTagConverter::toString(const ZoneTag& _zoneIndicator) const
{
    if(_zoneIndicator == ZoneTag::RAW)
    {
        return m_tagStringRaw;
    }
    else if(_zoneIndicator == ZoneTag::REFINED)
    {
        return m_tagStringRefined;
    }
    else if(_zoneIndicator == ZoneTag::COMPLIANT)
    {
        return m_tagStringCompliant;
	}
}

ot::ZoneTag ot::ZoneTagConverter::toZoneTag(const std::string& _zoneTagString) const
{
    if(_zoneTagString == m_tagStringRaw)
    {
        return ZoneTag::RAW;
    }
    else if(_zoneTagString == m_tagStringRefined)
    {
        return ZoneTag::REFINED;
    }
    else if(_zoneTagString == m_tagStringCompliant)
    {
        return ZoneTag::COMPLIANT;
	}
}
