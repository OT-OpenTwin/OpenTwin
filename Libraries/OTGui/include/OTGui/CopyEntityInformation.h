// @otlicense
// File: CopyEntityInformation.h
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
#include "OTCore/Point2D.h"
#include "OTCore/CoreTypes.h"
#include "OTCore/Serializable.h"
#include "OTGui/OTGuiAPIExport.h"

// std header
#include <string>

#pragma warning(disable:4251)

namespace ot {

	class OT_GUI_API_EXPORT CopyEntityInformation : public Serializable {
	public:
		CopyEntityInformation();
		CopyEntityInformation(const ConstJsonObject& _jsonObject);
		CopyEntityInformation(UID _uid, const std::string& _name, const std::string& _rawData = std::string());
		CopyEntityInformation(const CopyEntityInformation&) = default;
		CopyEntityInformation(CopyEntityInformation&&) = default;
		~CopyEntityInformation() = default;

		CopyEntityInformation& operator = (const CopyEntityInformation&) = default;
		CopyEntityInformation& operator = (CopyEntityInformation&&) = default;

		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		void setUid(UID _uid) { m_uid = _uid; };
		UID getUid(void) const { return m_uid; };

		void setName(const std::string& _name) { m_name = _name; };
		const std::string& getName(void) const { return m_name; };

		void setRawData(const std::string& _data) { m_rawData = _data; };
		void setRawData(std::string&& _data) { m_rawData = std::move(_data); };
		const std::string& getRawData(void) const { return m_rawData; };

	private:
		UID m_uid;
		std::string m_name;
		std::string m_rawData;
	};
}