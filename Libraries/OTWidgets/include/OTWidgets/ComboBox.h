// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetBase.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtWidgets/qcombobox.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT ComboBox : public QComboBox, public WidgetBase {
		Q_OBJECT
	public:
		ComboBox(QWidget* _parent = (QWidget*)nullptr);
		virtual ~ComboBox() {};

		virtual QWidget* getQWidget() override { return this; };
		virtual const QWidget* getQWidget() const override { return this; };

		bool isPopupVisible() const { return m_popupVisible; };

		virtual void showPopup() override;
		virtual void hidePopup() override;

	Q_SIGNALS:
		void returnPressed();

	public Q_SLOTS:
		void togglePopup();

	protected:
		virtual void keyPressEvent(QKeyEvent* _event) override;

	private:
		bool m_popupVisible;
	};

}
