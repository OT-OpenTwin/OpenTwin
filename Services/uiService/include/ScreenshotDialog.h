// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/Dialog.h"

namespace ot {

	class PixmapEditWidget;

	class ScreenshotDialog : public Dialog {
		Q_OBJECT
		OT_DECL_NOCOPY(ScreenshotDialog)
		OT_DECL_NOMOVE(ScreenshotDialog)
		OT_DECL_NODEFAULT(ScreenshotDialog)
	public:
		ScreenshotDialog(QWidget* _parent);
		virtual ~ScreenshotDialog();

		void setPixmap(const QPixmap& _pixmap);
		void setResultSize(const QSize& _size);
		QPixmap getResultPixmap() const;

	private:
		PixmapEditWidget* m_editor;

	};

}