//! @file GraphicsTriangleItemCfg.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date November 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/GraphicsItemCfg.h"
#include "OpenTwinCore/OTClassHelper.h"

#define OT_SimpleFactoryJsonKeyValue_GraphicsTriangleItemCfg "OT_GICTria"

namespace ot {

	class Painter2D;

	class OT_GUI_API_EXPORTONLY GraphicsTriangleItemCfg : public ot::GraphicsItemCfg {
		OT_DECL_NOCOPY(GraphicsTriangleItemCfg)
	public:
		enum TriangleDirection {
			Left,
			Up,
			Right,
			Down
		};

		static std::string triangleDirectionToString(TriangleDirection _direction);
		static TriangleDirection stringToTriangleDirection(const std::string& _str);

		GraphicsTriangleItemCfg(TriangleDirection _direction = Right);
		virtual ~GraphicsTriangleItemCfg();

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(OT_rJSON_val& _object) override;

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string simpleFactoryObjectKey(void) const override { return std::string(OT_SimpleFactoryJsonKeyValue_GraphicsTriangleItemCfg); };

		void setOutline(const ot::Border& _outline) { m_outline = _outline; };
		const ot::Border& outline(void) const { return m_outline; };

		void setBackgroundPainer(ot::Painter2D* _painter);
		ot::Painter2D* backgroundPainter(void) { return m_backgroundPainter; };

		void setSize(const ot::Size2DD& _size) { m_size = _size; };
		const ot::Size2DD& size(void) const { return m_size; };

		void setTriangleDirection(TriangleDirection _direction) { m_direction = _direction; };
		TriangleDirection triangleDirection(void) const { return m_direction; };

	private:
		ot::Border m_outline;
		ot::Size2DD m_size;
		ot::Painter2D* m_backgroundPainter;
		TriangleDirection m_direction;
	};

}
