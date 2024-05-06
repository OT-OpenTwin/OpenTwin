//! @file GraphicsBoxLayoutItemCfg.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/GraphicsLayoutItemCfg.h"

#define OT_FactoryKey_GraphicsGridLayoutItem "OT_GILayG"

namespace ot {

	class OT_GUI_API_EXPORT GraphicsGridLayoutItemCfg : public GraphicsLayoutItemCfg {
	public:
		GraphicsGridLayoutItemCfg(int _rows = 0, int _columns = 0);
		virtual ~GraphicsGridLayoutItemCfg();

		//! @brief Add the object contents to the provided JSON object.
		//! @param _document The JSON document (used to get the allocator).
		//! @param _object The JSON object to add the contents to.
		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object.
		//! @param _object The JSON object containing the information.
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types).
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		//! @brief Returns the key that is used to create an instance of this class in the simple factory.
		virtual std::string getFactoryKey(void) const override { return std::string(OT_FactoryKey_GraphicsGridLayoutItem); };

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