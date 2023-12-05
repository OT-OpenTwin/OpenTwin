#pragma once
#pragma warning(disable:4251)
// OpenTwin header
#include "OpenTwinCore/CoreAPIExport.h"
#include "OpenTwinCore/Serializable.h"
#include "OpenTwinCore/rJSON.h"
#include "OpenTwinCore/ReturnValues.h"

// std header
#include <string>

namespace ot
{
	class OT_CORE_API_EXPORT ReturnMessage : public ot::Serializable
	{
	public:
		enum ReturnMessageStatus {
			Ok, //! @brief Ok
			Failed //! @brief Error
		};

		static std::string statusToString(ReturnMessageStatus _status);
		static ReturnMessageStatus stringToStatus(const std::string& _status);

		//! @brief Create a ReturnMessage from the provided json string
		//! This function will create a new instance and and call set from json.
		//! If the provided json string is invalid the resulting return message will have the result failed and the what() will return the error string
		//! @param _json The json string
		static ReturnMessage fromJson(const std::string& _json);

		//! @brief Create a json string that can be used to create a ReturnMessage instance
		//! @param _status The status
		//! @param _what The message contents
		static std::string toJson(ReturnMessageStatus _status, const std::string& _what = std::string());

		//! @brief Constructor
		//! @param _status The status
		//! @param _what The message contents
		ReturnMessage(ReturnMessageStatus _status = ReturnMessageStatus::Ok, const std::string& _what = std::string());
		ReturnMessage(ot::ReturnValues& values);
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

		//! @brief Get the return message content
		std::string getWhat(void) const { return m_what; }

		//! @brief Get the return message status
		ReturnMessageStatus getStatus(void) const { return m_status; }

		const ReturnValues& getValues() const { return m_values; }

		//! @brief Returns a string representation of the status
		std::string getStatusString(void) const;

		//! @brief Returns a JSON String
		//! Will create a JSON document, call addToJSONObject and return the JSON string created by the document
		std::string toJson(void) const;

	private:
		ReturnMessageStatus m_status;
		std::string m_what;
		ot::ReturnValues m_values;
	};
}
