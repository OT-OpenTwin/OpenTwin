//! @file GraphicsGroupItem.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/GraphicsItemCfg.h"

#define OT_FactoryKey_GraphicsGroupItem "OT_GIGroup"

namespace ot {

	//! @class GraphicsGroupItem.
	//! @brief The graphics group item is used to group multiple items into one item.
	class OT_GUI_API_EXPORT GraphicsGroupItemCfg : public ot::GraphicsItemCfg {
	public:
		GraphicsGroupItemCfg();
		virtual ~GraphicsGroupItemCfg();

		//! \brief Creates a copy of this item.
		virtual GraphicsItemCfg* createCopy(void) const override;

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string getFactoryKey(void) const override { return std::string(OT_FactoryKey_GraphicsGroupItem); };

		//! @brief Will add the provided item to the group
		//! @param _item Item to add
		void addItem(ot::GraphicsItemCfg* _item) { m_items.push_back(_item); };

		//! @brief Returns a list with all items in the group
		const std::list<GraphicsItemCfg*>& items(void) const { return m_items; };

	private:
		void memClear(void);

		std::list<GraphicsItemCfg*> m_items;
	};

}
