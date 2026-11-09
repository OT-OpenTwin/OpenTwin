// @otlicense
// File: RAIIBase.h
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
#include "OTCore/CoreTypes.h"

namespace ot
{
	class OT_CORE_API_EXPORT RAIIBase
	{
		OT_DECL_NOCOPY(RAIIBase)
	protected:
		explicit RAIIBase();
		explicit RAIIBase(RAIIBase&& _other) noexcept : m_isValid(_other.m_isValid) { _other.m_isValid = false; };
		virtual ~RAIIBase() = default;

		RAIIBase& operator=(RAIIBase&& _other) noexcept = delete;

	public:

		//! @brief Apply and invalidate.
		void finalize();

		//! @brief Invalidate.
		void dismiss();

		virtual bool isValid() const { return m_isValid; };

	protected:
		virtual void execute() = 0;
		virtual void invalidate() {};
		
	private:
		bool m_isValid;
	};

}