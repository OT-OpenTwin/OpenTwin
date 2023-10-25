//! @file GraphicsBoxLayoutItemCfg.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/GraphicsLayoutItemCfg.h"

namespace ot {

	class OT_GUI_API_EXPORTONLY GraphicsBoxLayoutItemCfg : public GraphicsLayoutItemCfg {
	public:
		//! @brief Holds the item and its strech factor in the box
		//! If no item is set then its a stretch only
		typedef std::pair<GraphicsItemCfg*, int> itemStrechPair_t;

		GraphicsBoxLayoutItemCfg(ot::Orientation _orientation = ot::Horizontal);
		virtual ~GraphicsBoxLayoutItemCfg();

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(OT_rJSON_val& _object) override;

		virtual void addChildItem(ot::GraphicsItemCfg* _item) override;
		void addChildItem(ot::GraphicsItemCfg* _item, int _stretch);
		void addStrech(int _stretch = 1);
		const std::list<itemStrechPair_t>& items(void) const { return m_items; };

	private:
		void clearItems(void);

		ot::Orientation m_orientation;
		std::list<itemStrechPair_t> m_items; //! @brief Items and their stretch in the box (nullptr = stretch only)

		GraphicsBoxLayoutItemCfg(const GraphicsBoxLayoutItemCfg&) = delete;
		GraphicsBoxLayoutItemCfg& operator = (const GraphicsBoxLayoutItemCfg&) = delete;
	};

}