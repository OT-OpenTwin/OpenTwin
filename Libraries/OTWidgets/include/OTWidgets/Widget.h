//! @file Widget.h
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTObject.h"
#include "OTCore/BasicServiceInformation.h"
#include "OTGui/GuiTypes.h"
#include "OTWidgets/WidgetBase.h"

namespace ot {

	class OT_WIDGETS_API_EXPORT Widget : public OTObject, public WidgetBase {
		OT_DECL_NOCOPY(Widget)
		OT_DECL_NOMOVE(Widget)
	public:
		Widget();
		virtual ~Widget();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		void setWidgetOwner(const BasicServiceInformation& _owner) { m_owner = _owner; };
		const BasicServiceInformation& getWidgetOwner(void) const { return m_owner; };

		// ###########################################################################################################################################################################################################################################################################################################################

		// State management

		void setWidgetEnabled(bool _enabled, int _counter = 1);

		//! @brief Resets the disabled counter back to 0. (like no disabled was called)
		void resetWidgetDisabledCounter(void);

		void lockWidget(const LockTypeFlags& _flags, int _lockCount = 1);

		void unlockWidget(const LockTypeFlags& _flags, int _unlockCount = 1);

		void resetWidgetLockCounter(void);

	protected:
		//! @brief This method is called whenever the enabled state should be updated (e.g. setEnabled or setReadOnly).
		virtual void updateWidgetEnabledState(bool _enabled, bool _locked) = 0;

	private:
		void lockWidgetFlag(LockTypeFlag _flag, int _lockCount);
		void unlockWidgetFlag(LockTypeFlag _flag, int _unlockCount);

		void evaluateEnabledState(void);
		int& getLockCounter(LockTypeFlag _flag);

		BasicServiceInformation m_owner;
		bool m_isEnabled;
		bool m_isUnlocked;
		int m_disabledCounter;
		std::map<LockTypeFlag, int> m_lockCounter;
	};

}