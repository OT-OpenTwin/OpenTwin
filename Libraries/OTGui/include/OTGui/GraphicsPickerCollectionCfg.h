// @otlicense

#pragma once

// OpenTwin header
#include "OTGui/GraphicsPickerItemInfo.h"

// std header
#include <string>
#include <list>

#pragma warning(disable:4251)

namespace ot {
	
	//! @brief The GraphicsPickerCollectionCfg class is used to define a collection of graphics item information.
	class OT_GUI_API_EXPORT GraphicsPickerCollectionCfg : public ot::Serializable {
		OT_DECL_DEFCOPY(GraphicsPickerCollectionCfg)
		OT_DECL_DEFMOVE(GraphicsPickerCollectionCfg)
	public:
		GraphicsPickerCollectionCfg();
		GraphicsPickerCollectionCfg(const std::string& _collectionName, const std::string& _collectionTitle);
		virtual ~GraphicsPickerCollectionCfg() = default;

		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		//! @brief Set the collection name.
		//! The name must be unique within the parent collection.
		//! @param _name The name of the collection.
		void setName(const std::string& _name) { m_name = _name; };

		//! @brief Get the collection name.
		//! @ref getName
		const std::string& getName(void) const { return m_name; };

		//! @brief Set the collection title.
		//! The title is used for display purposes only.
		//! @param _title The title of the collection.
		void setTitle(const std::string& _title) { m_title = _title; };

		//! @brief Get the collection title.
		//! @ref getTitle
		const std::string& getTitle(void) const { return m_title; };

		//! @brief Add a child collection to this collection.
		//! @ref addChildCollection(GraphicsPickerCollectionCfg&&)
		//! @param _collectionNameAndTitle The name and title of the collection. The name must be unique within this collection.
		//! @return Reference to the added or merged collection.
		GraphicsPickerCollectionCfg& addChildCollection(const std::string& _collectionNameAndTitle) { return this->addChildCollection(_collectionNameAndTitle, _collectionNameAndTitle); };

		//! @brief Add a child collection to this collection.
		//! @ref addChildCollection(GraphicsPickerCollectionCfg&&)
		//! @param _collectionName The name of the collection. The name must be unique within this collection.
		//! @param _collectionTitle The title of the collection.
		//! @return Reference to the added or merged collection.
		GraphicsPickerCollectionCfg& addChildCollection(const std::string& _collectionName, const std::string& _collectionTitle);

		//! @brief Add a child collection to this collection.
		//! @ref addChildCollection(GraphicsPickerCollectionCfg&&)
		//! @param _child The child collection to add. The name of the collection must be unique within this collection.
		//! @return Reference to the added or merged collection.
		GraphicsPickerCollectionCfg& addChildCollection(const GraphicsPickerCollectionCfg& _child);

		//! @brief Add a child collection to this collection.
		//! If a child collection with the same name already exists, the two collections will be merged.
		//! @param _child The child collection to add. The name of the collection must be unique within this collection.
		//! @return Reference to the added or merged collection.
		GraphicsPickerCollectionCfg& addChildCollection(GraphicsPickerCollectionCfg&& _child);

		//! @brief Get the child collections of this collection.
		const std::list<GraphicsPickerCollectionCfg>& getChildCollections(void) const { return m_collections; };

		//! @brief Add an item to this collection.
		//! @ref addItem(GraphicsPickerItemInfo&&)
		//! @param _itemName The name of the item. The name must be unique within this collection.
		//! @param _itemTitle The title of the item.
		//! @param _previewIconPath The path to the preview icon of the item.
		//! @return Reference to the added or existing item.
		GraphicsPickerItemInfo& addItem(const std::string& _itemName, const std::string& _itemTitle, const std::string& _previewIconPath);

		//! @brief Add an item to this collection.
		//! @ref addItem(GraphicsPickerItemInfo&&)
		//! @param _itemInfo The item to add. The name of the item must be unique within this collection.
		//! @return Reference to the added or existing item.
		GraphicsPickerItemInfo& addItem(const GraphicsPickerItemInfo& _itemInfo);

		//! @brief Add an item to this collection.
		//! If an item with the same name already exists, the existing item will be returned.
		//! @param _itemInfo The item to add. The name of the item must be unique within this collection.
		//! @return Reference to the added or existing item.
		GraphicsPickerItemInfo& addItem(GraphicsPickerItemInfo&& _itemInfo);

		//! @brief Get the items of this collection.
		const std::list<GraphicsPickerItemInfo>& getItems(void) const { return m_items; };

		//! @brief Will merge this collection with the collection provided.
		//! New data will be added to this collection.
		void mergeWith(GraphicsPickerCollectionCfg&& _other);

		//! @brief Returns true if this collection does not contain any items and has only empty child collections.
		bool isEmpty() const;

		//! @brief Clear all items and child collections from this collection.
		void clear();

	private:
		std::list<GraphicsPickerCollectionCfg> m_collections;
		std::list<GraphicsPickerItemInfo> m_items;
		std::string m_name;
		std::string m_title;
	};
}