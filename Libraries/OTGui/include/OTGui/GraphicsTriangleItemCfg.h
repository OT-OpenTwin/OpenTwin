//! @file GraphicsTriangleItemCfg.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date November 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/Outline.h"
#include "OTGui/GraphicsItemCfg.h"
#include "OTCore/OTClassHelper.h"

#define OT_FactoryKey_GraphicsTriangleItem "OT_GITria"

namespace ot {

	class Painter2D;

	class OT_GUI_API_EXPORT GraphicsTriangleItemCfg : public ot::GraphicsItemCfg {
		OT_DECL_NOCOPY(GraphicsTriangleItemCfg)
	public:
		enum TriangleDirection {
			Left,
			Up,
			Right,
			Down
		};

		enum TriangleShape {
			Triangle,
			Kite,
			IceCone
		};

		static std::string triangleDirectionToString(TriangleDirection _direction);
		static TriangleDirection stringToTriangleDirection(const std::string& _str);

		static std::string triangleShapeToString(TriangleShape _shape);
		static TriangleShape stringToTriangleShape(const std::string& _shape);

		GraphicsTriangleItemCfg(TriangleDirection _direction = Right, TriangleShape _shape = Triangle);
		virtual ~GraphicsTriangleItemCfg();

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
		virtual std::string getFactoryKey(void) const override { return std::string(OT_FactoryKey_GraphicsTriangleItem); };

		void setOutline(const ot::OutlineF& _outline) { m_outline = _outline; };
		const ot::OutlineF& outline(void) const { return m_outline; };

		void setBackgroundPainer(ot::Painter2D* _painter);
		ot::Painter2D* backgroundPainter(void) { return m_backgroundPainter; };

		void setSize(const ot::Size2DD& _size) { m_size = _size; };
		const ot::Size2DD& size(void) const { return m_size; };

		void setTriangleShape(TriangleShape _shape) { m_shape = _shape; };
		TriangleShape triangleShape(void) const { return m_shape; };

		void setTriangleDirection(TriangleDirection _direction) { m_direction = _direction; };
		TriangleDirection triangleDirection(void) const { return m_direction; };

	private:
		ot::OutlineF m_outline;
		ot::Size2DD m_size;
		ot::Painter2D* m_backgroundPainter;
		TriangleDirection m_direction;
		TriangleShape m_shape;
	};

}
