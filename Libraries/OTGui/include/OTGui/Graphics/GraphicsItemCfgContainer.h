// @otlicense

#pragma once

// OpenTwin header
#include "OTGui/Graphics/GraphicsItemCfg.h"

namespace ot
{

	class OT_GUI_API_EXPORT GraphicsItemCfgContainer : public Serializable
	{
	public:
		GraphicsItemCfgContainer(GraphicsItemCfg* _item = (GraphicsItemCfg*)nullptr);
		GraphicsItemCfgContainer(const GraphicsItemCfgContainer& _other);
		GraphicsItemCfgContainer(GraphicsItemCfgContainer&& _other) noexcept;
		~GraphicsItemCfgContainer();

		GraphicsItemCfgContainer& operator = (const GraphicsItemCfgContainer& _other);
		GraphicsItemCfgContainer& operator = (GraphicsItemCfgContainer&& _other) noexcept;

		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		bool isEmpty() const { return m_item == nullptr; };
		void replace(GraphicsItemCfg* _item);
		GraphicsItemCfg* get() { return m_item; };
		const GraphicsItemCfg* get() const { return m_item; };
		GraphicsItemCfg* take() { GraphicsItemCfg* tmp = m_item; m_item = nullptr; return tmp; };

	private:
		GraphicsItemCfg* m_item;
	};

}