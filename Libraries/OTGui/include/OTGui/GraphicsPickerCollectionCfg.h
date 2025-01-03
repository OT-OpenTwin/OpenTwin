//! @file GraphicsPickerCollectionCfg.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/GraphicsPickerItemInformation.h"

// std header
#include <string>
#include <list>

#pragma warning(disable:4251)

namespace ot {
	class GraphicsItemCfg;

	class OT_GUI_API_EXPORT GraphicsPickerCollectionCfg : public ot::Serializable {
	public:
		GraphicsPickerCollectionCfg();
		GraphicsPickerCollectionCfg(const std::string& _collectionName, const std::string& _collectionTitle);
		GraphicsPickerCollectionCfg(const GraphicsPickerCollectionCfg& _other);
		virtual ~GraphicsPickerCollectionCfg();

		GraphicsPickerCollectionCfg& operator = (const GraphicsPickerCollectionCfg& _other);

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		void setName(const std::string& _name) { m_name = _name; };
		const std::string& getName(void) const { return m_name; };

		void setTitle(const std::string& _title) { m_title = _title; };
		const std::string& getTitle(void) const { return m_title; };

		void addChildCollection(GraphicsPickerCollectionCfg* _child);
		const std::list<GraphicsPickerCollectionCfg*>& getChildCollections(void) const { return m_collections; };

		void addItem(const std::string& _itemName, const std::string& _itemTitle, const std::string& _previewIconPath);
		void addItem(const GraphicsPickerItemInformation& _itemInfo);
		const std::list<GraphicsPickerItemInformation>& getItems(void) const { return m_items; };

		//! \brief Will merge this collection with the collection provided.
		//! New data will be added to this collection.
		void mergeWith(const GraphicsPickerCollectionCfg& _other);

	private:
		void memFree(void);

		std::list<GraphicsPickerCollectionCfg*> m_collections;
		std::list<GraphicsPickerItemInformation> m_items;
		std::string m_name;
		std::string m_title;
	};
}