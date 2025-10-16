//! @file GraphicsBoxLayoutItemCfg.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/GraphicsLayoutItemCfg.h"

namespace ot {

	class OT_GUI_API_EXPORT GraphicsGridLayoutItemCfg : public GraphicsLayoutItemCfg {
	public:
		static std::string className() { return "GraphicsGridLayoutItemCfg"; };

		GraphicsGridLayoutItemCfg(int _rows = 0, int _columns = 0);
		GraphicsGridLayoutItemCfg(const GraphicsGridLayoutItemCfg& _other);
		virtual ~GraphicsGridLayoutItemCfg();

		GraphicsGridLayoutItemCfg& operator = (const GraphicsGridLayoutItemCfg&) = delete;

		//! @brief Creates a copy of this item.
		virtual GraphicsItemCfg* createCopy() const override { return new GraphicsGridLayoutItemCfg(*this); };

		//! @brief Add the object contents to the provided JSON object.
		//! @param _document The JSON document (used to get the allocator).
		//! @param _object The JSON object to add the contents to.
		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object.
		//! @param _object The JSON object containing the information.
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types).
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		//! @brief Returns the key that is used to create an instance of this class in the simple factory.
		virtual std::string getFactoryKey() const override { return GraphicsGridLayoutItemCfg::className(); };

		int getRowCount() const { return m_rows; };
		int getColumnCount() const { return m_columns; };

		virtual void addChildItem(ot::GraphicsItemCfg* _item) override;
		void addChildItem(int _row, int _column, ot::GraphicsItemCfg* _item);
		const std::vector<std::vector<GraphicsItemCfg*>>& getItems() const { return m_items; };

		void setRowStretch(int _row, int _stretch);
		const std::vector<int>& getRowStretch() const { return m_rowStretch; };

		void setColumnStretch(int _column, int _stretch);
		const std::vector<int>& getColumnStretch() const { return m_columnStretch; };

	private:
		void clearAndResize();

		int m_rows;
		int m_columns;
		std::vector<int> m_rowStretch;
		std::vector<int> m_columnStretch;
		std::vector<std::vector<GraphicsItemCfg*>> m_items;
	};

}