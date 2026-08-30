// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/CoreTypes.h"

// std header
#include <list>

namespace ot
{
	class ObjectDestroyNotifier;

	class OT_CORE_API_EXPORT ObjectWithDestroyNotifier
	{
		OT_DECL_NOCOPY(ObjectWithDestroyNotifier)
		OT_DECL_NOMOVE(ObjectWithDestroyNotifier)
	public:
		ObjectWithDestroyNotifier();
		virtual ~ObjectWithDestroyNotifier();

		//! @brief Registers a notifier to be notified when this object is destroyed.
		//! The notifier will be notified when this object is destroyed.
		//! The notifier will be removed from the list of notifiers when it is destroyed.
		//! The caller keeps the ownership of the notifier.
		void addDestroyNotifier(ObjectDestroyNotifier* _notifier);

		//! @brief Unregisters a notifier from the list of notifiers.
		//! The object will deregister from the notifier.
		void removeDestroyNotifier(ObjectDestroyNotifier* _notifier);

		//! @brief Notifies all registered notifiers that this object is about to be destroyed.
		//! This should be called before the object is destroyed to allow the notifiers to 
		//! perform any necessary cleanup or actions before the actual object is destroyed.
		void notifyPreDestroy();

		//! @brief Removes all registered notifiers from the list of notifiers.
		//! The object will deregister from the notifier.
		void removeAllDestroyNotifiers();

	private:
		std::list<ObjectDestroyNotifier*> m_destroyNotifiers;
	};

}