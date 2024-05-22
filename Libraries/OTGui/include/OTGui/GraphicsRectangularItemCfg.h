//! @file GraphicsRectangularItemCfg.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/Outline.h"
#include "OTGui/GraphicsItemCfg.h"

#define OT_FactoryKey_GraphicsRectangularItem "OT_GIRect"

namespace ot {

	class Painter2D;

	class OT_GUI_API_EXPORT GraphicsRectangularItemCfg : public ot::GraphicsItemCfg {
	public:
		GraphicsRectangularItemCfg(ot::Painter2D* _backgroundPainter = (ot::Painter2D*)nullptr, int _cornerRadius = 0);
		virtual ~GraphicsRectangularItemCfg();

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
		virtual std::string getFactoryKey(void) const override { return std::string(OT_FactoryKey_GraphicsRectangularItem); };

		void setCornerRadius(int _px) { m_cornerRadius = _px; };
		int cornerRadius(void) const { return m_cornerRadius; };

		void setOutline(const ot::OutlineF& _outline) { m_outline = _outline; };
		const ot::OutlineF& outline(void) const { return m_outline; };

		void setBackgroundPainer(ot::Painter2D* _painter);
		ot::Painter2D* backgroundPainter(void) { return m_backgroundPainter; };

		void setSize(const ot::Size2DD& _size) { m_size = _size; };
		const ot::Size2DD& size(void) const { return m_size; };

	private:
		int m_cornerRadius;
		ot::OutlineF m_outline;
		ot::Size2DD m_size;
		ot::Painter2D* m_backgroundPainter;

		GraphicsRectangularItemCfg(GraphicsRectangularItemCfg&) = delete;
		GraphicsRectangularItemCfg& operator = (GraphicsRectangularItemCfg&) = delete;
	};

}
