#pragma once

// OpenTwin header
#include "OpenTwinCore/CoreAPIExport.h"
#include "OpenTwinCore/Serializable.h"
#include "OpenTwinCore/rJSON.h"

// std header
#include <string>

namespace ot
{
	class OT_CORE_API_EXPORT ReturnMessage : public ot::Serializable
	{
	public:
		enum ReturnMessageStatus {
			Ok,
			Failed
		};

		static std::string statusToString(ReturnMessageStatus _status);
		static ReturnMessageStatus stringToStatus(const std::string& _status);

		static ReturnMessage fromJson(const std::string& _json);

		ReturnMessage(ReturnMessageStatus _status = ReturnMessageStatus::Ok, const std::string& _what = std::string());
		ReturnMessage(const ReturnMessage& _other);
		~ReturnMessage() {};
		ReturnMessage& operator = (const ReturnMessage& _other);
		ReturnMessage& operator = (const char * _what);
		ReturnMessage& operator = (const std::string& _what);
		ReturnMessage& operator = (ReturnMessageStatus _status);

		bool operator == (const ReturnMessageStatus _status) const;
		bool operator == (const ReturnMessage& _other) const;

		bool operator != (const ReturnMessageStatus _status) const;
		bool operator != (const ReturnMessage& _other) const;

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(OT_rJSON_val& _object) override;

		std::string getWhat() const { return m_what; }
		ReturnMessageStatus getStatus() const { return m_status; }

		//! @brief Returns a JSON String
		//! Will create a JSON document, call addToJSONObject and return the JSON string created by the document
		std::string toJson(void) const;

	private:
		ReturnMessageStatus m_status;
		std::string m_what;
	};

	/*
	struct ReturnStatus
	{
		static const std::string Ok() { return "OK"; };
		static const std::string Failed() { return "FAILED"; };
		static const std::string True() { return "TRUE"; };
		static const std::string False() { return "FALSE"; };
	};
	*/
}
