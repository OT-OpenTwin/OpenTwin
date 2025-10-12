//! @file SelectionInformation.h
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

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