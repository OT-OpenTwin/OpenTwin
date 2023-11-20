//! @file GraphicsCollectionCfg.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/OTGuiAPIExport.h"
#include "OTCore/Serializable.h"

// std header
#include <string>
#include <list>

#pragma warning(disable:4251)

namespace ot {
	class GraphicsItemCfg;

	class OT_GUI_API_EXPORT GraphicsCollectionCfg : public ot::Serializable {
	public:
		GraphicsCollectionCfg();
		GraphicsCollectionCfg(const std::string& _collectionName, const std::string& _collectionTitle);
		virtual ~GraphicsCollectionCfg();

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(OT_rJSON_val& _object) override;

		void setName(const std::string& _name) { m_name = _name; };
		const std::string& name(void) const { return m_name; };

		void setTitle(const std::string& _title) { m_title = _title; };
		const std::string& title(void) const { return m_title; };

		void addChildCollection(GraphicsCollectionCfg* _child);
		const std::list<GraphicsCollectionCfg*>& childCollections(void) const { return m_collections; };

		void addItem(GraphicsItemCfg* _item);
		const std::list<GraphicsItemCfg*>& items(void) const { return m_items; };

	private:
		void memFree(void);

		std::list<GraphicsCollectionCfg*> m_collections;
		std::list<GraphicsItemCfg*> m_items;
		std::string m_name;
		std::string m_title;
	};
}