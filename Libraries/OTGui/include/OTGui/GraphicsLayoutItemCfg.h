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
		virtual void addFactoryKey(OT_rJSON_doc& _document, OT_rJSON_val& _object) const = 0;

	private:
		GraphicsLayoutItemCfg(const GraphicsLayoutItemCfg&) = delete;
		GraphicsLayoutItemCfg& operator = (const GraphicsLayoutItemCfg&) = delete;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class OT_GUI_API_EXPORT GraphicsBoxLayoutItemCfg : public GraphicsLayoutItemCfg {
	public:
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

	private:
		void clearItems(void);

		ot::Orientation m_orientation;
		typedef std::pair<GraphicsItemCfg*, int> itemStrechPair_t;
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

	protected:
		virtual void addFactoryKey(OT_rJSON_doc& _document, OT_rJSON_val& _object) const override;

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

	protected:
		virtual void addFactoryKey(OT_rJSON_doc& _document, OT_rJSON_val& _object) const override;

	private:
		GraphicsHBoxLayoutItemCfg(const GraphicsHBoxLayoutItemCfg&) = delete;
		GraphicsHBoxLayoutItemCfg& operator = (const GraphicsHBoxLayoutItemCfg&) = delete;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class OT_GUI_API_EXPORT GraphicsGridLayoutItemCfg : public GraphicsLayoutItemCfg {
	public:
		GraphicsGridLayoutItemCfg(int _rows = 0, int _columns = 0);
		virtual ~GraphicsGridLayoutItemCfg() {};

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(OT_rJSON_val& _object) override;

		virtual void addChildItem(ot::GraphicsItemCfg* _item) override;
		void addChildItem(int _row, int _column, ot::GraphicsItemCfg* _item);

	protected:
		virtual void addFactoryKey(OT_rJSON_doc& _document, OT_rJSON_val& _object) const override;

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