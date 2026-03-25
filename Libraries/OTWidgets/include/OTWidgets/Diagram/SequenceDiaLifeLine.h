// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/Diagram/SequenceDiaGraphicsItem.h"

namespace ot {

	class GraphicsGroupItem;
	
	class OT_WIDGETS_API_EXPORT SequenceDiaLifeLine : public SequenceDiaGraphicsItem
	{
		OT_DECL_NOCOPY(SequenceDiaLifeLine)
		OT_DECL_NOMOVE(SequenceDiaLifeLine)
		OT_DECL_NODEFAULT(SequenceDiaLifeLine)
	public:
		SequenceDiaLifeLine(const std::string& _name);
		virtual ~SequenceDiaLifeLine();

		virtual GraphicsItem* getGraphicsItem() const override;
		virtual QRectF getBoundingRect() const override;

	private:
		GraphicsGroupItem* m_rootGroup;

	};
}