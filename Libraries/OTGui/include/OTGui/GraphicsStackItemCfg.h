//! @file GraphicsItemCfg.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/GraphicsItemCfg.h"

#define OT_SimpleFactoryJsonKeyValue_GraphicsStackItemCfg "OT_GICStack"

namespace ot {

	class OT_GUI_API_EXPORTONLY GraphicsStackItemCfg : public ot::GraphicsItemCfg {
	public:
		struct GraphicsStackItemCfgEntry {
			ot::GraphicsItemCfg* item;
			bool isMaster;
			bool isSlave;
		};

		GraphicsStackItemCfg();
		virtual ~GraphicsStackItemCfg();

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string simpleFactoryObjectKey(void) const override { return std::string(OT_SimpleFactoryJsonKeyValue_GraphicsStackItemCfg); };

		//! @brief Will add the provided item on the top of the stack (Zn)
		//! If multiple master items are provided, the biggest size will be used
		//! @param _item Item to add
		//! @param _isMaster If true, all slave items will be expanded to match the master size
		//! @param _isSlave If true, the item will adjust its size according to the size of the master item(s)
		void addItemTop(ot::GraphicsItemCfg* _item, bool _isMaster, bool _isSlave);

		//! @brief Will add the provided item at the bottom of the stack (Z0)
		//! If multiple master items are provided, the biggest size will be used
		//! @param _item Item to add
		//! @param _isMaster If true, all slave items will be expanded to match the master size
		//! @param _isSlave If true, the item will adjust its size according to the size of the master item(s)
		void addItemBottom(ot::GraphicsItemCfg* _item, bool _isMaster, bool _isSlave);

		//! @brief Returns a list with all items in the stack
		//! 0: bottom, n: top
		const std::list<GraphicsStackItemCfgEntry>& items(void) const { return m_items; };

	private:
		void memClear(void);

		std::list<GraphicsStackItemCfgEntry> m_items;

	};

}
