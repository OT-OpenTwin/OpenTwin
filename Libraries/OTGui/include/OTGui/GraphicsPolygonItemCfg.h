//! @file GraphicsPolygonItemCfg.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/Path2D.h"
#include "OTGui/GraphicsItemCfg.h"
#include "OTCore/OTClassHelper.h"

#define OT_SimpleFactoryJsonKeyValue_GraphicsPolygonItemCfg "OT_GICPoly"

namespace ot {

	class OT_GUI_API_EXPORT GraphicsPolygonItemCfg : public ot::GraphicsItemCfg {
		OT_DECL_NOCOPY(GraphicsPolygonItemCfg)
	public:
		GraphicsPolygonItemCfg();
		virtual ~GraphicsPolygonItemCfg();

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string simpleFactoryObjectKey(void) const override { return std::string(OT_SimpleFactoryJsonKeyValue_GraphicsPolygonItemCfg); };

		void setPath(const Path2D& _path) { m_path = _path; };
		Path2D& path(void) { return m_path; };
		const Path2D& path(void) const { return m_path; };

	private:
		Path2D m_path;
	};

}
