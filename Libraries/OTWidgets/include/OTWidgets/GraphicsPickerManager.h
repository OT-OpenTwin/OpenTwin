//! @file GraphicsPickerManager.h
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/BasicServiceInformation.h"
#include "OTGui/GraphicsPickerCollectionCfg.h"
#include "OTWidgets/GraphicsPicker.h"

// std header
#include <map>

namespace ot {

	class OT_WIDGETS_API_EXPORT GraphicsPickerManager {
		OT_DECL_NOCOPY(GraphicsPickerManager)
		OT_DECL_DEFMOVE(GraphicsPickerManager)
	public:
		GraphicsPickerManager(GraphicsPicker* _picker = (GraphicsPicker*)nullptr);
		virtual ~GraphicsPickerManager() {};

		void setCurrentOwner(const BasicServiceInformation& _owner);
		const BasicServiceInformation& getCurrentOwner() const { return m_currentOwner; };

		void setPicker(GraphicsPicker* _picker) { m_picker = _picker; };
		void forgetPicker() { m_picker = nullptr; };

		void addCollections(const std::list<GraphicsPickerCollectionCfg>& _collections, const BasicServiceInformation& _owner);
		void addCollections(std::list<GraphicsPickerCollectionCfg>&& _collections, const BasicServiceInformation& _owner);
		
		const std::list<GraphicsPickerCollectionCfg>& getCollections(const BasicServiceInformation& _owner) const;

		void clear();
		
		void clearPicker();

	private:
		struct PickerInfo {
			PickerInfo() = default;
			PickerInfo(GraphicsPickerCollectionCfg&& _collection) : collection(std::move(_collection)) {}
			
			GraphicsPickerCollectionCfg collection;
			GraphicsPicker::PickerState state;
		};

		void applyCurrentOwner();

		PickerInfo& getOwnerInfo(const BasicServiceInformation& _owner);
		const PickerInfo& getOwnerInfo(const BasicServiceInformation& _owner) const;

		GraphicsPicker* m_picker;
		BasicServiceInformation m_currentOwner;

		std::map<BasicServiceInformation, PickerInfo> m_collections;
		PickerInfo m_emptyInfo;
	};

}
