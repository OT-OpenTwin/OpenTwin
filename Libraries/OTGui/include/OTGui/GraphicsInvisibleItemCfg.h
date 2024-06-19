//! @file GraphicsInvisibleItemCfg.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date June 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/Outline.h"
#include "OTGui/GraphicsItemCfg.h"

#define OT_FactoryKey_GraphicsInvisibleItem "OT_GIInvis"

namespace ot {

	class Painter2D;

	class OT_GUI_API_EXPORT GraphicsInvisibleItemCfg : public ot::GraphicsItemCfg {
		OT_DECL_NOCOPY(GraphicsInvisibleItemCfg)
	public:
		GraphicsInvisibleItemCfg(const ot::Size2DD& _size = ot::Size2DD());
		virtual ~GraphicsInvisibleItemCfg();

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
		virtual std::string getFactoryKey(void) const override { return std::string(OT_FactoryKey_GraphicsInvisibleItem); };

		void setSize(const ot::Size2DD& _size) { m_size = _size; };
		const ot::Size2DD& getSize(void) const { return m_size; };

	private:
		ot::Size2DD m_size;
	};

}
