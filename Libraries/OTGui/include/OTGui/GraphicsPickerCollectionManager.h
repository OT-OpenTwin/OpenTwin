//! @file GraphicsPickerCollectionManager.h
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTCore/BasicServiceInformation.h"
#include "OTGui/GraphicsPickerCollectionCfg.h"

// std header
#include <map>

namespace ot {
	
	class OT_GUI_API_EXPORT GraphicsPickerCollectionManager {
		OT_DECL_NOCOPY(GraphicsPickerCollectionManager)
	public:
		GraphicsPickerCollectionManager() {};
		virtual ~GraphicsPickerCollectionManager() {};

		void setCurrentOwner(const BasicServiceInformation& _owner) { m_currentOwner = _owner; };
		const BasicServiceInformation& getCurrentOwner(void) const { return m_currentOwner; };

		void addCollection(const GraphicsPickerCollectionCfg& _collection, const BasicServiceInformation& _owner);
		void addCollections(const std::list<GraphicsPickerCollectionCfg*>& _collections, const BasicServiceInformation& _owner);
		const std::list<GraphicsPickerCollectionCfg*>& getCollections(const BasicServiceInformation& _owner) const;

		void clear(void);

	private:
		GraphicsPickerCollectionCfg& getOwnerRootCollection(const BasicServiceInformation& _owner);
		const GraphicsPickerCollectionCfg& getOwnerRootCollection(const BasicServiceInformation& _owner) const;

		BasicServiceInformation m_currentOwner;
		std::map<BasicServiceInformation, GraphicsPickerCollectionCfg> m_collections;
		GraphicsPickerCollectionCfg m_emptyCollection;
	};

}