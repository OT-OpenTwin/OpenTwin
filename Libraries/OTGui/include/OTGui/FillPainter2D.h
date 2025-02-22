//! @file FillPainter2D.h
//! @author Alexander Kuester (alexk95)
//! @date March 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Color.h"
#include "OTGui/Painter2D.h"

#define OT_FactoryKey_FillPainter2D "OT_P2DFill"

namespace ot {
	
	class OT_GUI_API_EXPORT FillPainter2D : public ot::Painter2D {
	public:
		FillPainter2D();
		FillPainter2D(ot::DefaultColor _color);
		FillPainter2D(int _r, int _g, int _b, int _a = 255);
		FillPainter2D(const ot::Color& _color);
		virtual ~FillPainter2D();

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string getFactoryKey(void) const override { return std::string(OT_FactoryKey_FillPainter2D); };

		virtual std::string generateQss(void) const override;

		virtual ot::Color getDefaultColor(void) const override;

		virtual bool isEqualTo(const Painter2D* _other) const override;

		void setColor(const ot::Color& _color) { m_color = _color; };
		const ot::Color& getColor(void) const { return m_color; };

	private:
		ot::Color m_color;

		FillPainter2D(const FillPainter2D&) = delete;
		FillPainter2D& operator = (const FillPainter2D&) = delete;
	};

}