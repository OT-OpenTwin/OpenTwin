//! @file OnePropertyDialog.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/OnePropertyDialogCfg.h"
#include "OTWidgets/Dialog.h"

namespace ot {

	class PropertyInput;

	class OT_WIDGETS_API_EXPORT OnePropertyDialog : public Dialog {
		OT_DECL_NOCOPY(OnePropertyDialog)
	public:
		OnePropertyDialog(const OnePropertyDialogCfg& _config, QWidget* _parent = (QWidget*)nullptr);
		virtual ~OnePropertyDialog();

		void addPropertyInputValueToJson(ot::JsonValue& _object, const char* _memberNameValue, ot::JsonAllocator& _allocator);
		QVariant getCurrentValue(void) const;

		bool valueHasChanged(void) const { return m_changed; };

	private Q_SLOTS:
		void slotValueChanged(void);
		void slotConfirm(void);
		
	private:
		PropertyInput* m_input;
		bool m_changed;

		OnePropertyDialog() = delete;
	};
}
