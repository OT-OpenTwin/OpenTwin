// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetTypes.h"

// Qt header
#include <QtCore/qobject.h>
#include <QtCore/qvariant.h>

namespace ot
{
	class OT_WIDGETS_API_EXPORT RandomizerWheelItem
	{
		OT_DECL_NOCOPY(RandomizerWheelItem)
		OT_DECL_NOMOVE(RandomizerWheelItem)
		OT_DECL_NODEFAULT(RandomizerWheelItem)
	public:
		explicit RandomizerWheelItem(const QString& _text, const QVariant& _data = QVariant());
		virtual ~RandomizerWheelItem() = default;

		void setText(const QString& _text) { m_text = _text; };
		const QString& getText() const { return m_text; };

		void setData(const QVariant& _data) { m_data = _data; };
		const QVariant& getData() const { return m_data; };

	private:
		QString m_text;
		QVariant m_data;
	};
}