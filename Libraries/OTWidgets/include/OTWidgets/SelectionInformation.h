// @otlicense
// File: SelectionInformation.h
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
#include "OTWidgets/WidgetTypes.h"

namespace ot {

	class OT_WIDGETS_API_EXPORT SelectionInformation : public Serializable {
		OT_DECL_DEFCOPY(SelectionInformation)
		OT_DECL_DEFMOVE(SelectionInformation)
	public:
		SelectionInformation() = default;
		virtual ~SelectionInformation() = default;

		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		void addSelectedNavigationItem(UID _uid) { m_uids.push_back(_uid); m_uids.unique(); };
		void addSelectedNavigationItems(UIDList&& _uids) { m_uids.splice(m_uids.end(), std::move(_uids)); m_uids.unique(); };
		void setSelectedNavigationItems(const UIDList& _uids) { m_uids = _uids; m_uids.unique(); };
		void removeSelectedNavigationItem(UID _uid);
		const UIDList& getSelectedNavigationItems(void) const { return m_uids; };

	private:
		UIDList m_uids;

	};

}