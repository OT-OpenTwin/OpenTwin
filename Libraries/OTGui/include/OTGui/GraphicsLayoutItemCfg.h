//! @file GraphcisLayoutItemCfg.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/GraphicsItemCfg.h"
#include "OTGui/GuiTypes.h"

// std header
#include <list>
#include <vector>

#define OT_SimpleFactoryJsonKeyValue_GraphicsVBoxLayoutItemCfg "OT_GICLayV"
#define OT_SimpleFactoryJsonKeyValue_GraphicsHBoxLayoutItemCfg "OT_GICLayH"
#define OT_SimpleFactoryJsonKeyValue_GraphicsGridLayoutItemCfg "OT_GICLayG"

namespace ot {

	class OT_GUI_API_EXPORTONLY GraphicsLayoutItemCfg : public GraphicsItemCfg {
	public:
		GraphicsLayoutItemCfg() {};
		virtual ~GraphicsLayoutItemCfg();

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(OT_rJSON_val& _object) override;

		virtual void addChildItem(ot::GraphicsItemCfg* _item) = 0;

	protected:
		
	private:
		GraphicsLayoutItemCfg(const GraphicsLayoutItemCfg&) = delete;
		GraphicsLayoutItemCfg& operator = (const GraphicsLayoutItemCfg&) = delete;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class OT_GUI_API_EXPORTONLY GraphicsBoxLayoutItemCfg : public GraphicsLayoutItemCfg {
	public:
		//! @brief Holds the item and its strech factor in the box
		//! If no item is set then its a stretch only
		typedef std::pair<GraphicsItemCfg*, int> itemStrechPair_t;

		GraphicsBoxLayoutItemCfg(ot::Orientation _orientation = ot::Horizontal);
		virtual ~GraphicsBoxLayoutItemCfg();

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(OT_rJSON_val& _object) override;

		virtual void addChildItem(ot::GraphicsItemCfg* _item) override;
		void addChildItem(ot::GraphicsItemCfg* _item, int _stretch);
		void addStrech(int _stretch = 1);
		const std::list<itemStrechPair_t>& items(void) const { return m_items; };

	private:
		void clearItems(void);

		ot::Orientation m_orientation;
		std::list<itemStrechPair_t> m_items; //! @brief Items and their stretch in the box (nullptr = stretch only)
		
		GraphicsBoxLayoutItemCfg(const GraphicsBoxLayoutItemCfg&) = delete;
		GraphicsBoxLayoutItemCfg& operator = (const GraphicsBoxLayoutItemCfg&) = delete;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class OT_GUI_API_EXPORTONLY GraphicsVBoxLayoutItemCfg : public GraphicsBoxLayoutItemCfg {
	public:
		GraphicsVBoxLayoutItemCfg() : GraphicsBoxLayoutItemCfg(ot::Vertical) {};
		virtual ~GraphicsVBoxLayoutItemCfg() {};

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string simpleFactoryObjectKey(void) const override { return std::string(OT_SimpleFactoryJsonKeyValue_GraphicsVBoxLayoutItemCfg); };

	private:
		GraphicsVBoxLayoutItemCfg(const GraphicsVBoxLayoutItemCfg&) = delete;
		GraphicsVBoxLayoutItemCfg& operator = (const GraphicsVBoxLayoutItemCfg&) = delete;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class OT_GUI_API_EXPORTONLY GraphicsHBoxLayoutItemCfg : public GraphicsBoxLayoutItemCfg {
	public:
		GraphicsHBoxLayoutItemCfg() : GraphicsBoxLayoutItemCfg(ot::Horizontal) {};
		virtual ~GraphicsHBoxLayoutItemCfg() {};

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string simpleFactoryObjectKey(void) const override { return std::string(OT_SimpleFactoryJsonKeyValue_GraphicsHBoxLayoutItemCfg); };

	private:
		GraphicsHBoxLayoutItemCfg(const GraphicsHBoxLayoutItemCfg&) = delete;
		GraphicsHBoxLayoutItemCfg& operator = (const GraphicsHBoxLayoutItemCfg&) = delete;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class OT_GUI_API_EXPORTONLY GraphicsGridLayoutItemCfg : public GraphicsLayoutItemCfg {
	public:
		GraphicsGridLayoutItemCfg(int _rows = 0, int _columns = 0);
		virtual ~GraphicsGridLayoutItemCfg();

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(OT_rJSON_val& _object) override;

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string simpleFactoryObjectKey(void) const override { return std::string(OT_SimpleFactoryJsonKeyValue_GraphicsGridLayoutItemCfg); };

		int rowCount(void) const { return m_rows; };
		int columnCount(void) const { return m_columns; };

		virtual void addChildItem(ot::GraphicsItemCfg* _item) override;
		void addChildItem(int _row, int _column, ot::GraphicsItemCfg* _item);
		const std::vector<std::vector<GraphicsItemCfg*>>& items(void) const { return m_items; };

		void setRowStretch(int _row, int _stretch);
		const std::vector<int>& rowStretch(void) const { return m_rowStretch; };

		void setColumnStretch(int _column, int _stretch);
		const std::vector<int>& columnStretch(void) const { return m_columnStretch; };

	private:
		void clearAndResize(void);

		int m_rows;
		int m_columns;
		std::vector<int> m_rowStretch;
		std::vector<int> m_columnStretch;
		std::vector<std::vector<GraphicsItemCfg*>> m_items;

		GraphicsGridLayoutItemCfg(const GraphicsGridLayoutItemCfg&) = delete;
		GraphicsGridLayoutItemCfg& operator = (const GraphicsGridLayoutItemCfg&) = delete;
	};


}