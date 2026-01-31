// @otlicense
// File: SequenceDiaFunctionCfg.h
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

// OpenTwin header
#include "OTGui/Diagram/SequenceDiaAbstractCallCfg.h"

namespace ot {

	class OT_GUI_API_EXPORT SequenceDiaFunctionCfg : public Serializable {
		OT_DECL_NOCOPY(SequenceDiaFunctionCfg)
	public:
		SequenceDiaFunctionCfg() = default;
		SequenceDiaFunctionCfg(const ConstJsonObject& _jsonObject);
		SequenceDiaFunctionCfg(SequenceDiaFunctionCfg&& _other) noexcept;
		virtual ~SequenceDiaFunctionCfg();

		SequenceDiaFunctionCfg& operator=(SequenceDiaFunctionCfg&& _other) noexcept;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Serialization

		virtual void addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _jsonObject) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		void clear();

		bool isValid() const;

		void setLifeLine(const std::string& _lifeLine) { m_lifeLine = _lifeLine; };
		void setLifeLine(std::string&& _lifeLine) { m_lifeLine = std::move(_lifeLine); };
		const std::string& getLifeLine() const { return m_lifeLine; };

		void setName(const std::string& _name) { m_name = _name; };
		void setName(std::string&& _name) { m_name = std::move(_name); };
		const std::string& getName() const { return m_name; };

		void setDefaultText(const std::string& _text) { m_defaultText = _text; };
		void setDefaultText(std::string&& _text) { m_defaultText = std::move(_text); };
		const std::string& getDefaultText() const { return m_defaultText; };

		void setFilePath(const std::string& _filePath) { m_filePath = _filePath; };
		void setFilePath(std::string&& _filePath) { m_filePath = std::move(_filePath); };
		const std::string& getFilePath() const { return m_filePath; };

		void setLineNr(size_t _lineNr) { m_lineNr = _lineNr; };
		size_t getLineNr() const { return m_lineNr; };

		void setReturnValue(const std::string& _returnValue) { m_returnValue = _returnValue; };
		void setReturnValue(std::string&& _returnValue) { m_returnValue = std::move(_returnValue); };
		const std::string& getReturnValue() const { return m_returnValue; };

		//! @brief Add a call to the function.
		//! The function takes ownership of the call object.
		//! @param _call Pointer to the call to add.
		void addCall(SequenceDiaAbstractCallCfg* _call);
		const std::list<SequenceDiaAbstractCallCfg*>& getCalls() const { return m_calls; };

	private:
		std::string m_lifeLine;
		std::string m_name;
		std::string m_defaultText;

		std::string m_filePath;
		size_t m_lineNr = 0;

		std::list<SequenceDiaAbstractCallCfg*> m_calls;
		std::string m_returnValue;
	};

}