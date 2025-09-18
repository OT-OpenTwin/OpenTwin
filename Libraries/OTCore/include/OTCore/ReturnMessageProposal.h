#pragma once

// OpenTwin header
#include "OTSystem/Flags.h"
#include "OTCore/JSON.h"
#include "OTCore/Logger.h"

// std header
#include <string>

namespace ot {

	class ReturnMessage2 {
	public:
		enum ReturnMessageFlag {
			Ok                 = 0 << 0, //! @brief Ok.
			GeneralError       = 1 << 0, //! @brief General error during execution.
			IOError            = 1 << 1, //! @brief Error while transmitting data.
			DeserializeFailed  = 1 << 2, //! @brief Error while deserializing data.

			//! @brief All failed and error flags.
			Failed             = GeneralError | IOError | DeserializeFailed,
		};

		typedef Flags<ReturnMessageFlag> ReturnMessageStatus;

		static std::string flagToString(ReturnMessageFlag _flag) {
			switch (_flag) {
			case ot::ReturnMessage2::Ok: return "Ok";
			case ot::ReturnMessage2::GeneralError: return "GeneralError";
			case ot::ReturnMessage2::IOError: return "IOError";
			case ot::ReturnMessage2::DeserializeFailed: return "DeserializeFailed";
			case ot::ReturnMessage2::Failed: return "Failed";
			default:
				OT_LOG_E("Unknown return message flag \"" + std::to_string(static_cast<int>(_flag)) + "\"");
				return "DeserializeFailed";
			}
		}

		static ReturnMessageFlag stringToFlag(const std::string& _flag) {
			if (_flag == flagToString(ReturnMessageFlag::Ok)) { return ReturnMessageFlag::Ok; }
			else if (_flag == flagToString(ReturnMessageFlag::GeneralError)) { return ReturnMessageFlag::GeneralError; }
			else if (_flag == flagToString(ReturnMessageFlag::IOError)) { return ReturnMessageFlag::IOError; }
			else if (_flag == flagToString(ReturnMessageFlag::DeserializeFailed)) { return ReturnMessageFlag::DeserializeFailed; }
			else if (_flag == flagToString(ReturnMessageFlag::Failed)) { return ReturnMessageFlag::Failed; }
			else {
				OT_LOG_E("Unknown return message flag \"" + _flag + "\"");
				return ReturnMessageFlag::DeserializeFailed;
			}
		}

		static std::list<std::string> statusToStringList(const ReturnMessageStatus& _status) {
			std::list<std::string> result;

			if (_status & ReturnMessageFlag::Ok) { result.push_back(flagToString(ReturnMessageFlag::Ok)); }
			if (_status & ReturnMessageFlag::GeneralError) { result.push_back(flagToString(ReturnMessageFlag::GeneralError)); }
			if (_status & ReturnMessageFlag::IOError) { result.push_back(flagToString(ReturnMessageFlag::IOError)); }
			if (_status & ReturnMessageFlag::DeserializeFailed) { result.push_back(flagToString(ReturnMessageFlag::DeserializeFailed)); }
			if (_status & ReturnMessageFlag::Failed) { result.push_back(flagToString(ReturnMessageFlag::Failed)); }

			return result;
		}

		static ReturnMessageStatus stringListToStatus(const std::list<std::string>& _status) {
			ReturnMessageStatus result = ReturnMessageFlag::Ok;
			for (const std::string& flag : _status) {
				result |= stringToFlag(flag);
			}
			return result;
		}

		static ReturnMessage2 fromJson(const std::string& _json) {
			ReturnMessage2 result(ReturnMessageFlag::Ok);

			if (!_json.empty()) {
				result = ReturnMessageFlag::DeserializeFailed;

				JsonDocument doc;
				if (doc.fromJson(_json)) {
					result = stringListToStatus(json::getStringList(doc, "Status"));
					result = json::getString(doc, "Text");
					result.m_importDataObject = json::getObject(doc, "Data");
				}
			}


			return std::move(result);
		}

		ReturnMessage2(const ReturnMessageStatus& _status = ReturnMessageFlag::Ok, const std::string& _text = std::string())
			: m_status(_status), m_text(_text), m_importDataObject(getEmptyObject())
		{

		}

		ReturnMessage2(ReturnMessage2&& _other) noexcept :
			m_status(_other.m_status), m_text(std::move(_other.m_text)), m_importDataObject(std::move(_other.m_importDataObject))
		{}

		ReturnMessage2& operator = (ReturnMessage2&& _other) noexcept {
			if (this != &_other) {
				m_status = _other.m_status;
				m_text = std::move(_other.m_text);
				m_importDataObject = std::move(_other.m_importDataObject);
			}

			return *this;
		}

		ReturnMessage2& operator = (ReturnMessageStatus _status) {
			m_status = _status;
			return *this;
		}

		ReturnMessage2& operator = (const char * _text) {
			m_text = _text;
			return *this;
		}

		ReturnMessage2& operator = (const std::string& _text) {
			m_text = _text;
			return *this;
		}

		operator std::string(void) noexcept {
			return this->toJson();
		}

		//! @brief Exports the ReturnMessage as a json string.
		//! @note The importDataObject will not be serialized.
		std::string toJson(void) const {
			JsonDocument doc;
			JsonObject obj;
			return this->toJson(doc, obj);
		}

		//! @brief Exports the ReturnMessage as a json string.
		//! @note The importDataObject will not be serialized.
		//! @param _document Data will be written here before serializing.
		std::string toJson(JsonDocument& _document) const {
			JsonObject obj;
			return this->toJson(_document, obj);
		}

		//! @brief Exports the ReturnMessage as a json string.
		//! @note The importDataObject will not be serialized.
		//! @param _document Data will be written here before serializing.
		//! @param _dataObject The data object.
		std::string toJson(JsonDocument& _document, JsonObject& _dataObject) const {
			_document.AddMember("Status", JsonArray(statusToStringList(m_status), _document.GetAllocator()), _document.GetAllocator());
			_document.AddMember("Text", JsonString(m_text, _document.GetAllocator()), _document.GetAllocator());
			_document.AddMember("Data", _dataObject, _document.GetAllocator());

			return _document.toJson();
		}

		void setStatus(ReturnMessageStatus _status) { m_status = _status; };
		ReturnMessageStatus getStatus(void) const { return m_status; };

		void setText(const std::string& _text) { m_text = _text; };
		const std::string& getText(void) const { return m_text; };

		//! @brief Returns a readable object which contains the deserialized data object.
		//! @note The object will not be concidered when serializing the ReturnMessage.
		const ConstJsonObject& getData(void) const { return m_importDataObject; };

	private:
		ReturnMessageStatus m_status; //! @brief Status of the message.
		std::string m_text; //! @brief Text of the message.

		ConstJsonObject m_importDataObject; //! @brief The object which contains the deserialized data object.
		
		void setImportDataObject(const ConstJsonObject& _obj) { m_importDataObject = _obj; }

		static const ConstJsonObject& getEmptyObject(void) {
			static JsonDocument g_doc;
			return g_doc.getConstObject();
		}
	};

}

OT_ADD_FLAG_FUNCTIONS(ot::ReturnMessage2::ReturnMessageFlag)