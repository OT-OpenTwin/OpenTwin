//! @file SelectionInformation.h
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/CoreTypes.h"
#include "OTCore/Serializable.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

namespace ot {

	class OT_WIDGETS_API_EXPORT SelectionInformation : public Serializable {
	public:
		SelectionInformation() = default;
		SelectionInformation(const SelectionInformation&) = default;
		SelectionInformation(SelectionInformation&&) = default;
		virtual ~SelectionInformation() = default;

		SelectionInformation& operator = (const SelectionInformation&) = default;
		SelectionInformation& operator = (SelectionInformation&&) = default;

		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		void addSelectedNavigationItem(UID _uid) { m_uids.push_back(_uid); };
		void setSelectedNavigationItems(const UIDList& _uids) { m_uids = _uids; };
		const UIDList& getSelectedNavigationItems(void) const { return m_uids; };

	private:
		UIDList m_uids;

	};

}