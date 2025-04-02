// OpenTwin header
#include "OTCore/ReturnMessage.h"
#include "OTCore/Logger.h"

std::string ot::ReturnMessage::statusToString(ot::ReturnMessage::ReturnMessageStatus _status) {
	switch (_status)
	{
	case ot::ReturnMessage::Ok: return "Ok";
	case ot::ReturnMessage::Failed: return "Failed";
	default:
		OT_LOG_EA("Unknown Return Message Status");
		return "Failed";
	}
}

ot::ReturnMessage::ReturnMessageStatus ot::ReturnMessage::stringToStatus(const std::string& _status) {
	if (_status == statusToString(ReturnMessage::Ok)) return ReturnMessage::Ok;
	else if (_status == statusToString(ReturnMessage::Failed)) return ReturnMessage::Failed;
	else {
		OT_LOG_EAS("Unknown Return Message Status \"" + _status + "\"");
		return ReturnMessage::Failed;
	}
}

ot::ReturnMessage ot::ReturnMessage::fromJson(const std::string& _json) {
	if (_json.empty()) {
		return ReturnMessage(ot::ReturnMessage::Ok);
	}
	ReturnMessage msg(ot::ReturnMessage::Failed);

	JsonDocument doc;
	if (!doc.fromJson(_json)) {
		msg = "Failed to deserialize return message \"" + _json + "\": Document is not an object";
		return msg;
	}

	if (doc.IsObject()) {
		try {
			msg.setFromJsonObject(doc.GetConstObject());
		}
		catch (const std::exception& _e) {
			msg = "Failed to deserialize return message \"" + _json + "\" with error: "  + std::string(_e.what());
		}
	}
	else {
		msg = "Failed to deserialize return message \"" + _json + "\": Invalid format";
	}
	return msg;
}

std::string ot::ReturnMessage::toJson(ot::ReturnMessage::ReturnMessageStatus _status, const std::string& _what) {
	ReturnMessage msg(_status, _what);
	return msg.toJson();
}

std::string ot::ReturnMessage::toJson(ReturnMessageStatus _status, const ot::JsonDocument& _document) {
	ReturnMessage msg(_status, _document.toJson());
	return msg.toJson();
}

ot::ReturnMessage::ReturnMessage(ReturnMessageStatus _status, const std::string& _what) : m_status(_status), m_what(_what) {}


ot::ReturnMessage::ReturnMessage(ot::ReturnValues& values)
	: m_status(ot::ReturnMessage::ReturnMessageStatus::Ok), m_values(values)
{}

ot::ReturnMessage::ReturnMessage(ot::ReturnValues && values)
	: m_status(ot::ReturnMessage::ReturnMessageStatus::Ok), m_values(std::move(values))
{
}

ot::ReturnMessage::ReturnMessage(ReturnMessageStatus _status, const ot::JsonDocument& _document) : m_status(_status) {
	m_what = _document.toJson();
}

ot::ReturnMessage& ot::ReturnMessage::operator = (const ReturnMessage& _other) {
	if (this != &_other) {
		m_status = _other.m_status;
		m_what = _other.m_what;
		m_values = _other.m_values;
	}
	return *this;
}

ot::ReturnMessage& ot::ReturnMessage::operator=(ReturnMessage&& _other) noexcept
{
	if (this != &_other) {
		m_status = std::move(_other.m_status);
		m_what = std::move(_other.m_what);
		m_values = std::move(_other.m_values);
	}
	return *this;
}

ot::ReturnMessage& ot::ReturnMessage::operator = (const char* _what) {
	this->m_what = _what;
	return *this;
}

ot::ReturnMessage& ot::ReturnMessage::operator = (const std::string& _what) {
	this->m_what = _what;
	return *this;
}

ot::ReturnMessage& ot::ReturnMessage::operator = (ReturnMessageStatus _status) {
	this->m_status = _status;
	return *this;
}

bool ot::ReturnMessage::operator == (const ReturnMessageStatus _status) const {
	return this->m_status == _status;
}

bool ot::ReturnMessage::operator == (const ReturnMessage& _other) const {
	return this->m_status == _other.m_status && this->m_what == _other.m_what && this->m_values == _other.m_values;
}

bool ot::ReturnMessage::operator != (const ReturnMessageStatus _status) const {
	return this->m_status != _status;
}

bool ot::ReturnMessage::operator != (const ReturnMessage& _other) const {
	return this->m_status != _other.m_status || this->m_what != _other.m_what || this->m_values != _other.m_values;
}

void ot::ReturnMessage::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	_object.AddMember("Status", JsonString(this->statusToString(m_status), _allocator), _allocator);
	_object.AddMember("What", JsonString(m_what, _allocator), _allocator);
	if (m_values.getNumberOfEntries() != 0)
	{
		ot::JsonValue values;
		values.SetObject();
		m_values.addToJsonObject(values, _allocator);
		_object.AddMember(ot::JsonValue("Values",_allocator), std::move(values), _allocator);
	}
}

void ot::ReturnMessage::setFromJsonObject(const ConstJsonObject& _object) {
	m_what = json::getString(_object, "What");
	m_status = stringToStatus(json::getString(_object, "Status"));
	
	if(_object.HasMember("Values")) {
		const auto& temp = _object["Values"];
		m_values.setFromJsonObject(temp.GetObject());
	}

}

std::string ot::ReturnMessage::getStatusString(void) const {
	return ReturnMessage::statusToString(m_status);
}

std::string ot::ReturnMessage::toJson(void) const {
	JsonDocument doc;
	this->addToJsonObject(doc, doc.GetAllocator());
	return doc.toJson();
}