//! @file GraphicsEditorPackage.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/OTGuiAPIExport.h"
#include "OpenTwinCore/Serializable.h"
#include "OpenTwinCore/SimpleFactory.h"

// std header
#include <list>

namespace ot {

	class GraphicsCollectionCfg;
	
	class OT_GUI_API_EXPORT GraphicsEditorPackage : public ot::Serializable {
	public:
		GraphicsEditorPackage();
		virtual ~GraphicsEditorPackage();

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(OT_rJSON_val& _object) override;

		void addCollection(GraphicsCollectionCfg* _collection);
		const std::list<GraphicsCollectionCfg*>& collections(void) const { return m_collections; };

	private:
		void memFree(void);

		std::list< GraphicsCollectionCfg*> m_collections;

		GraphicsEditorPackage(const GraphicsEditorPackage&) = delete;
		GraphicsEditorPackage& operator = (const GraphicsEditorPackage&) = delete;
	};
}