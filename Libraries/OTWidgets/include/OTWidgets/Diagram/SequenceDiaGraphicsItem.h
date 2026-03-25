// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetTypes.h"
#include "OTWidgets/Graphics/GraphicsItem.h"

namespace ot {

	class OT_WIDGETS_API_EXPORT SequenceDiaGraphicsItem
	{
		OT_DECL_DEFCOPY(SequenceDiaGraphicsItem)
		OT_DECL_DEFMOVE(SequenceDiaGraphicsItem)
	public:
		SequenceDiaGraphicsItem();
		virtual ~SequenceDiaGraphicsItem() = default;

		virtual GraphicsItem* getGraphicsItem() const = 0;

		virtual void stretchToIdeal() {};
		virtual QRectF getBoundingRect() const = 0;

	private:

	};
}