//! @file PropertyPainter2D.h
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Property.h"
#include "OTGui/OTGuiAPIExport.h"

#define OT_PROPERTY_TYPE_Painter2D "Painter2D"

namespace ot {

	class Painter2D;

	class OT_GUI_API_EXPORT PropertyPainter2D : public Property {
		OT_DECL_NOCOPY(PropertyPainter2D)
	public:
		PropertyPainter2D(const PropertyPainter2D* _other);
		PropertyPainter2D(const PropertyBase& _base);

		//! @brief Creates new instance.
		//! Object creates and takes ownership of the painter.
		PropertyPainter2D(PropertyFlags _flags = PropertyFlags(NoFlags));

		//! @brief Creates new instance.
		//! Object takes ownership of the painter.
		PropertyPainter2D(Painter2D* _painter, PropertyFlags _flags = PropertyFlags(NoFlags));

		//! @brief Creates new instance.
		//! Object creates copy the painter.
		PropertyPainter2D(const Painter2D* _painter, PropertyFlags _flags = PropertyFlags(NoFlags));

		//! @brief Creates new instance.
		//! Object takes ownership of the painter.
		PropertyPainter2D(const std::string& _name, Painter2D* _painter, PropertyFlags _flags = PropertyFlags(NoFlags));

		//! @brief Creates new instance.
		//! Object creates copy the painter.
		PropertyPainter2D(const std::string& _name, const Painter2D* _painter, PropertyFlags _flags = PropertyFlags(NoFlags));
		virtual ~PropertyPainter2D();

		virtual std::string getPropertyType(void) const override { return OT_PROPERTY_TYPE_Painter2D; };

		virtual Property* createCopy(void) const override;

		//! @brief Set the painter for this property.
		//! Object takes ownership of the painter.
		void setPainter(Painter2D* _painter);
		const Painter2D* getPainter(void) const { return m_painter; };

	protected:
		//! @brief Add the property data to the provided JSON object
		//! The property type is already added
		//! @param _object Json object reference
		//! @param _allocator Allocator
		virtual void getPropertyData(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! @brief Set the property data from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setPropertyData(const ot::ConstJsonObject& _object) override;

	private:
		Painter2D* m_painter;
	};

}