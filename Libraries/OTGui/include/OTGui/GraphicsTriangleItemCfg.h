// @otlicense

//! @file GraphicsTriangleItemCfg.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date November 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/PenCfg.h"
#include "OTGui/GraphicsItemCfg.h"

namespace ot {

	class Painter2D;

	class OT_GUI_API_EXPORT GraphicsTriangleItemCfg : public ot::GraphicsItemCfg {
		OT_DECL_NOMOVE(GraphicsTriangleItemCfg)
	public:
		static std::string className() { return "GraphicsTriangleItemCfg"; };

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
		GraphicsTriangleItemCfg(const GraphicsTriangleItemCfg& _other);
		virtual ~GraphicsTriangleItemCfg();

		GraphicsTriangleItemCfg& operator = (const GraphicsTriangleItemCfg&) = delete;

		//! @brief Creates a copy of this item.
		virtual GraphicsItemCfg* createCopy(void) const override { return new GraphicsTriangleItemCfg(*this); };

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string getFactoryKey(void) const override { return GraphicsTriangleItemCfg::className(); };

		void setOutline(const ot::PenFCfg& _outline) { m_outline = _outline; };
		const PenFCfg& getOutline(void) const { return m_outline; };

		void setBackgroundPainer(ot::Painter2D* _painter);
		const ot::Painter2D* getBackgroundPainter(void) const { return m_backgroundPainter; };

		void setSize(const ot::Size2DD& _size) { m_size = _size; };
		const ot::Size2DD& getSize(void) const { return m_size; };

		void setTriangleShape(TriangleShape _shape) { m_shape = _shape; };
		TriangleShape getTriangleShape(void) const { return m_shape; };

		void setTriangleDirection(TriangleDirection _direction) { m_direction = _direction; };
		TriangleDirection getTriangleDirection(void) const { return m_direction; };

	private:
		ot::PenFCfg m_outline;
		ot::Size2DD m_size;
		ot::Painter2D* m_backgroundPainter;
		TriangleDirection m_direction;
		TriangleShape m_shape;
	};

}
