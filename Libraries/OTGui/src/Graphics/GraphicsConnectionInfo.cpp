// @otlicense

// OpenTwin header
#include "OTGui/Graphics/GraphicsConnectionInfo.h"

ot::GraphicsConnectionInfo::GraphicsConnectionInfo(UID _originUid, const std::string& _originConnectableName, UID _destinationUid, const std::string& _destinationConnectableName)
	: m_originUID(_originUid), m_originConnectable(_originConnectableName), m_destUID(_destinationUid), m_destConnectable(_destinationConnectableName)
{}

ot::GraphicsConnectionInfo::GraphicsConnectionInfo(const ConstJsonObject & _jsonObject)
	: GraphicsConnectionInfo()
{
	setFromJsonObject(_jsonObject);
}

void ot::GraphicsConnectionInfo::addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const
{
	_jsonObject.AddMember("OriginUID", m_originUID, _allocator);
	_jsonObject.AddMember("OriginConnectable", JsonString(m_originConnectable, _allocator), _allocator);
	_jsonObject.AddMember("DestUID", m_destUID, _allocator);
	_jsonObject.AddMember("DestConnectable", JsonString(m_destConnectable, _allocator), _allocator);
}

void ot::GraphicsConnectionInfo::setFromJsonObject(const ConstJsonObject& _jsonObject)
{
	m_originUID = json::getUInt64(_jsonObject, "OriginUID");
	m_originConnectable = json::getString(_jsonObject, "OriginConnectable");
	m_destUID = json::getUInt64(_jsonObject, "DestUID");
	m_destConnectable = json::getString(_jsonObject, "DestConnectable");
}

ot::GraphicsConnectionInfo ot::GraphicsConnectionInfo::getReversedConnectionInfo() const
{
	GraphicsConnectionInfo ret(*this);

	ret.setOriginUid(getDestinationUid());
	ret.setOriginConnectable(getDestinationConnectable());

	ret.setDestinationUid(getOriginUid());
	ret.setDestinationConnectable(getOriginConnectable());

	return ret;
}

std::string ot::GraphicsConnectionInfo::createConnectionKey() const
{
	return std::to_string(m_originUID) + m_originConnectable + std::to_string(m_destUID) + m_destConnectable;
}

std::string ot::GraphicsConnectionInfo::createConnectionKeyReverse() const
{
	return std::to_string(m_destUID) + m_destConnectable + std::to_string(m_originUID) + m_originConnectable;
}