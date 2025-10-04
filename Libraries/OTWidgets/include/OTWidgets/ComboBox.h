//! @file ComboBox.h
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

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

		virtual QWidget* getQWidget(void) override { return this; };
		virtual const QWidget* getQWidget(void) const override { return this; };

		bool isPopupVisible(void) const { return m_popupVisible; };

		virtual void showPopup(void) override;
		virtual void hidePopup(void) override;

	public Q_SLOTS:
		void togglePopup(void);

	private:
		bool m_popupVisible;
	};

}
