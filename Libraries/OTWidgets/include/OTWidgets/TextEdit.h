// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetBase.h"

// Qt header
#include <QtWidgets/qtextedit.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT TextEdit : public QTextEdit, public ot::WidgetBase {
		Q_OBJECT
		OT_DECL_NOCOPY(TextEdit)
		OT_DECL_NOMOVE(TextEdit)
	public:
		TextEdit(QWidget* _parent = (QWidget*)nullptr);
		virtual ~TextEdit();

		//! @brief Returns a pointer to the root widget of this object
		virtual QWidget* getQWidget(void) override { return this; };
		virtual const QWidget* getQWidget(void) const override { return this; };

	};

}
