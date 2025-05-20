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
		SelectionInformation();
		SelectionInformation(const SelectionInformation& _other);
		SelectionInformation(SelectionInformation&& _other) noexcept;
		virtual ~SelectionInformation();

		SelectionInformation& operator = (const SelectionInformation& _other);
		SelectionInformation& operator = (SelectionInformation&& _other) noexcept;

		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		void addSelectedNavigationItem(UID _uid) { m_uids.push_back(_uid); };
		void addSelectedNavigationItems(UIDList&& _uids) { m_uids.splice(m_uids.end(), std::move(_uids)); };
		void setSelectedNavigationItems(const UIDList& _uids) { m_uids = _uids; };
		const UIDList& getSelectedNavigationItems(void) const { return m_uids; };

	private:
		UIDList m_uids;

	};

}