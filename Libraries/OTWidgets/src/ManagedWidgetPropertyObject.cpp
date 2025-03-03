//! @file ManagedWidgetPropertyObject.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/WidgetPropertyManager.h"
#include "OTWidgets/ManagedWidgetPropertyObject.h"

ot::ManagedWidgetPropertyObject::ManagedWidgetPropertyObject() :
	ManagedPropertyObject(new WidgetPropertyManager)
{
}

ot::ManagedWidgetPropertyObject::ManagedWidgetPropertyObject(ManagedWidgetPropertyObject&& _other) noexcept :
	ManagedPropertyObject(std::move(_other))
{}

ot::ManagedWidgetPropertyObject& ot::ManagedWidgetPropertyObject::operator=(ManagedWidgetPropertyObject&& _other) noexcept {
	ManagedPropertyObject::operator=(std::move(_other));
	return *this;
}
