// OpenTwin header
#include "OpenTwinCore/ReturnMessage.h"
#include "OpenTwinCore/Logger.h"

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
	OT_rJSON_parseDOC(doc, _json.c_str());
	ReturnMessage msg;
	msg.setFromJsonObject(doc);
	return msg;
}

std::string ot::ReturnMessage::toJson(ot::ReturnMessage::ReturnMessageStatus _status, const std::string& _what) {
	ReturnMessage msg(_status, _what);
	return msg.toJson();
}

ot::ReturnMessage::ReturnMessage(ReturnMessageStatus _status, const std::string& _what) : m_status(_status), m_what(_what) {}

ot::ReturnMessage::ReturnMessage(const ReturnMessage& _other) : m_status(_other.m_status), m_what(_other.m_what) {}

ot::ReturnMessage& ot::ReturnMessage::operator = (const ReturnMessage& _other) {
	if (this != &_other) {
		m_status = _other.m_status;
		m_what = _other.m_what;
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
	return this->m_status == _other.m_status && this->m_what == _other.m_what;
}

bool ot::ReturnMessage::operator != (const ReturnMessageStatus _status) const {
	return this->m_status != _status;
}

bool ot::ReturnMessage::operator != (const ReturnMessage& _other) const {
	return this->m_status != _other.m_status || this->m_what != _other.m_what;
}

void ot::ReturnMessage::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	ot::rJSON::add(_document, _object, "Status", statusToString(m_status));
	ot::rJSON::add(_document, _object, "What", m_what);
}

void ot::ReturnMessage::setFromJsonObject(OT_rJSON_val& _object) {
	m_what = ot::rJSON::getString(_object, "What");
	m_status = stringToStatus(ot::rJSON::getString(_object, "Status"));
}

std::string ot::ReturnMessage::getStatusString(void) const {
	return ReturnMessage::statusToString(m_status);
}

std::string ot::ReturnMessage::toJson(void) const {
	OT_rJSON_createDOC(doc);
	this->addToJsonObject(doc, doc);
	return ot::rJSON::toJSON(doc);
}