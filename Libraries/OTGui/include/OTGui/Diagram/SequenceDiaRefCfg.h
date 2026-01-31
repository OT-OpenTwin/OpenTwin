// @otlicense
// File: SequenceDiaRefCfg.h
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
#include "OTCore/Serializable.h"
#include "OTGui/GuiTypes.h"

namespace ot {

	class OT_GUI_API_EXPORT SequenceDiaRefCfg : public Serializable {
		OT_DECL_NOCOPY(SequenceDiaRefCfg)
		OT_DECL_DEFMOVE(SequenceDiaRefCfg)
	public:
		// ###########################################################################################################################################################################################################################################################################################################################

		// Constructor / Destructor

		SequenceDiaRefCfg() = default;
		SequenceDiaRefCfg(const ConstJsonObject& _jsonObject);
		virtual ~SequenceDiaRefCfg() = default;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Serialization

		virtual void addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _jsonObject) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		bool isValid() const { return !m_name.empty() && !m_function.empty(); };

		void setName(const std::string& _name) { m_name = _name; };
		const std::string& getName() const { return m_name; };

		void setFunctionName(const std::string& _function) { m_function = _function; };
		const std::string& getFunctionName() const { return m_function; };

		void setFilePath(const std::string& _filePath) { m_filePath = _filePath; };
		const std::string& getFilePath() const { return m_filePath; };

		void setLineNr(size_t _lineNr) { m_lineNr = _lineNr; };
		size_t getLineNr() const { return m_lineNr; };

	private:
		std::string m_name;
		std::string m_function;

		std::string m_filePath;
		size_t m_lineNr = 0;
	};

}