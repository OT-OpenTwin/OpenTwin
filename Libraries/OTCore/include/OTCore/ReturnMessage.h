// @otlicense
// File: ReturnMessage.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#pragma once
#pragma warning(disable:4251)

#include "OTCore/Serializable.h"
#include "OTCore/ReturnValues.h"

// std header
#include <string>

namespace ot
{
	class OT_CORE_API_EXPORT ReturnMessage : public ot::Serializable
	{
	public:
		using Serializable::toJson;

		enum ReturnMessageStatus {
			Ok,     //! @brief Ok
			Failed, //! @brief Error
			True,
			False
		};

		static std::string statusToString(ReturnMessageStatus _status);
		static ReturnMessageStatus stringToStatus(const std::string& _status);

		//! @brief Create a ReturnMessage from the provided json string.
		//! This function will create a new instance and and call set from json.
		//! If the provided json string is invalid the resulting return message will have the result Failed and the what() will return the error string.
		//! If the provided json string is empty the resulting return message will have the result Ok.
		//! @param _json The json string.
		static ReturnMessage fromJson(const std::string& _json);

		//! @brief Create a json string that can be used to create a ReturnMessage instance
		//! @param _status The status
		//! @param _what The message contents
		static std::string toJson(ReturnMessageStatus _status, const std::string& _what = std::string());

		//! @brief Create a json string that can be used to create a ReturnMessage instance
		//! @param _status Message status
		//! @param _document JSON document that will be set as message
		static std::string toJson(ReturnMessageStatus _status, const ot::JsonDocument& _document);

		//! @brief Constructor
		//! @param _status The status
		//! @param _what The message contents
		ReturnMessage(ReturnMessageStatus _status = ReturnMessageStatus::Ok, const std::string& _what = std::string());

		ReturnMessage(ot::ReturnValues& values);
		ReturnMessage(ot::ReturnValues&& values);

		//! @brief Constructor
		//! @param _status The status
		//! @param _document The document that will be serialized and set as what()
		ReturnMessage(ReturnMessageStatus _status, const ot::JsonDocument& _document);
		
		
		ReturnMessage(const ReturnMessage& _other) = default;
		ReturnMessage(ReturnMessage&& _other) = default;

		~ReturnMessage() {};
		ReturnMessage& operator = (const ReturnMessage& _other);
		ReturnMessage& operator = (ReturnMessage&& _other) noexcept;
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
		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		void setWhat(const std::string& _what) { m_what = _what; };

		//! @brief Get the return message content
		const std::string& getWhat() const { return m_what; }

		void setStatus(const ReturnMessageStatus& _status) { m_status = _status; };

		//! @brief Get the return message status
		ReturnMessageStatus getStatus() const { return m_status; };

		bool isOk() const { return m_status == ReturnMessageStatus::Ok; };

		void setValues(const ReturnValues& _values) { m_values = _values; };
		
		void setValues(ReturnValues&& _values) { m_values = std::move(_values); };

		ReturnValues& getValues() { return m_values; }

		//! @brief Returns a string representation of the status
		std::string getStatusString(void) const;

	private:
		ReturnMessageStatus m_status;
		std::string m_what;
		ot::ReturnValues m_values;
	};
}
