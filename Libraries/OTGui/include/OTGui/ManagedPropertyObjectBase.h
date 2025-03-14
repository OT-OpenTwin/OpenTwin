//! @file ManagedPropertyRegistrarBase.h
//! @author Alexander Kuester (alexk95)
//! @date March 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OT header
#include "OTCore/Rect.h"
#include "OTCore/Size2D.h"
#include "OTCore/Point2D.h"
#include "OTCore/OTAssert.h"
#include "OTCore/CoreTypes.h"
#include "OTGui/PropertyInt.h"
#include "OTGui/PropertyBool.h"
#include "OTGui/PropertyPath.h"
#include "OTGui/FillPainter2D.h"
#include "OTGui/PropertyGroup.h"
#include "OTGui/PropertyColor.h"
#include "OTGui/PropertyDouble.h"
#include "OTGui/PropertyString.h"
#include "OTGui/PropertyManager.h"
#include "OTGui/PropertyPainter2D.h"
#include "OTGui/PropertyStringList.h"
#include "OTGui/ManagedPropertyObject.h"
#include "OTGui/ManagedPropertyRegistrar.h"
#include "OTGui/PropertyReadCallbackNotifier.h"
#include "OTGui/PropertyWriteCallbackNotifier.h"
#include "OTGui/ManagedSpecialPropertyRegistrar.h"

// std header
#include <string>
#include <optional>
#include <functional>

// ###########################################################################################################################################################################################################################################################################################################################

// Property base macros

//! @def OT_MANAGED_PROPERTY_GENERAL_GROUP_NAME
//! @brief General group name used for ungrouped properties.
#define OT_MANAGED_PROPERTY_GENERAL_GROUP_NAME "General"

//! @def OT_MANAGED_PROPERTY_REGISTRAR_NAME
//! @brief Is used by the default macros to generate the regisrar object name for a given property name.
//! @param ___propertyObjectName The name is used for the generated instance name.
#define OT_MANAGED_PROPERTY_REGISTRAR_NAME(___propertyObjectName) m_intern_##___propertyObjectName##Registrar

//! @def OT_CREATE_AND_REGISTER_MANAGED_PROPERTY
//! @brief Create a property and registr it at the PropertyManager.
//! The registrar will register the property at the ot::PropertyManager upon object creation.
//! The resulting code will have the following structure: <br>
//!  <br>
//! Example with "OT_CREATE_AND_REGISTER_MANAGED_PROPERTY(Bool, MyBool, "General", false)": <br>
//! 
//!		private: <br>
//!			ot::ManagedPropertyRegistrar<ot::PropertyBool> m_intern_MyBool {
//!				this,
//!				"General",
//!				new ot::PropertyBool { "MyBool", false }
//!			}
//! @param ___propertyType Type of the property. Must match naming convention of OpenTwin properties.
//! @param ___propertyObjectName Name that will be used for object related objects and methods. (e.g. 'MyVal' will result in 'getMyVal' and so on).
//! @param ___propertyName Name of the property. Will be used to set the name of the created property.
//! @param ___propertyGroup Name of group to store the property at.
//! @param __VA_ARGS__ Initializer list for property.
#define OT_CREATE_AND_REGISTER_MANAGED_PROPERTY(___propertyType, ___propertyObjectName, ___propertyGroup, ___propertyName, ...) \
	private: ot::ManagedPropertyRegistrar<ot::Property##___propertyType> OT_MANAGED_PROPERTY_REGISTRAR_NAME(___propertyObjectName) { \
		this, \
		___propertyGroup, \
		new ot::Property##___propertyType{ ___propertyName, __VA_ARGS__ } \
	}

//! @def OT_MANAGED_PROPERTY_SETTER_CALLBACK_TYPE
//! @brief Declaration of property write callback type.
#define OT_MANAGED_PROPERTY_SETTER_CALLBACK_TYPE(___propertyType) std::optional<ot::PropertyWriteCallbackNotifier<ot::Property##___propertyType>::CallbackType>

//! @def OT_MANAGED_PROPERTY_GETTER_CALLBACK_TYPE
//! @brief Declaration of property read callback type.
#define OT_MANAGED_PROPERTY_GETTER_CALLBACK_TYPE std::optional<ot::PropertyReadCallbackNotifier::CallbackType>

//! @def OT_ADD_MANAGED_PROPERTY_SETTER
//! @brief Add setter method to access a created property.
//! @param ___score Scope that will be used to put the methods in (i.e. public, private, protected).
//! @param ___propertyType Type of the property. Must match naming convention of OpenTwin properties.
//! @param ___propertyObjectName Name that will be used for object related objects and methods. (e.g. 'MyVal' will result in 'getMyVal' and so on).
//! @param ___propertyName Name of the property. Will be used to set the name of the created property.
//! @param ___propertyGroup Name of group to store the property at.
//! @param ___setterType Value type that is used for the setter method (e.g. int or const std::string&).
//! @param ___propertySetter Property manager setter method name that will be called to set the value of a property (see: ot::PropertyManager). The set value must match the ___setterType.
//! @param ___setterCompletedCallback Will be called at the end of the setter method after the property was created/updated.
//! @param __VA_ARGS__ Initializer list for property.
//! @ref OT_ADD_MANAGED_PROPERTY_GETTER
//! @ref OT_ADD_MANAGED_PROPERTY_SETTERGETTER
//! @ref ot::PropertyManager
//! @ref ot::ManagedPropertyRegistrar
#define OT_ADD_MANAGED_PROPERTY_SETTER(___scope, ___propertyType, ___propertyObjectName, ___propertyGroup, ___propertyName, ___setterType, ___propertySetter, ___setterCompletedCallback, ...) \
	private: \
		friend class ot::PropertyWriteCallbackNotifier<ot::Property##___propertyType>; \
		ot::PropertyWriteCallbackNotifier<ot::Property##___propertyType> notify##___propertyObjectName##WriteWrapper { \
			[=]() -> ot::PropertyManager* { return this->getPropertyManager(); }, \
			___propertyGroup + std::string("/") + ___propertyName, \
			OT_MANAGED_PROPERTY_SETTER_CALLBACK_TYPE(___propertyType)(___setterCompletedCallback) \
		}; \
	___scope: \
		void set##___propertyObjectName(___setterType _value) { \
			this->getPropertyManager()->##___propertySetter(___propertyGroup, ___propertyName, _value); \
		}

//! @def OT_ADD_MANAGED_PROPERTY_GETTER
//! @brief Add getter method to access a created property.
//! @note Note that the property must be created (by calling set once) before get will successfully return a value.
//! This can be archieved by either using a registrar or by setting up the properties for a serialized document.
//! @param ___score Scope that will be used to put the methods in (i.e. public, private, protected).
//! @param ___propertyType Type of the property. Must match naming convention of OpenTwin properties.
//! @param ___propertyObjectName Name that will be used for object related objects and methods. (e.g. 'MyVal' will result in 'getMyVal' and so on).
//! @param ___propertyName Name of the property. Will be used to set the name of the created property.
//! @param ___propertyGroup Name of group to store the property at.
//! @param ___getterType Value type that is used for the getter method (e.g. int or const std::string&).
//! @param ___propertyGetter Property manager getter method name that will be called to get the value of a property (see: ot::PropertyManager). The returned value must match the ___getterType.
//! @param ___getterConstDecl Will be added to the end of the getter method. Should be either kept empty or const.
//! @param ___getterStartedCallback Will be called at the beginning of the getter method before accessing the property.
//! @param __VA_ARGS__ Initializer list for property.
//! @ref OT_ADD_MANAGED_PROPERTY_SETTER
//! @ref OT_ADD_MANAGED_PROPERTY_SETTERGETTER
//! @ref ot::PropertyManager
//! @ref ot::ManagedPropertyRegistrar
#define OT_ADD_MANAGED_PROPERTY_GETTER(___scope, ___propertyType, ___propertyObjectName, ___propertyGroup, ___propertyName, ___getterType, ___propertyGetter, ___getterConstDecl, ___getterStartedCallback, ...) \
	private: \
		friend class ot::PropertyReadCallbackNotifier; \
		ot::PropertyReadCallbackNotifier notify##___propertyObjectName##ReadWrapper { \
			[=]() -> ot::PropertyManager* { return this->getPropertyManager(); }, \
			___propertyGroup + std::string("/") + ___propertyName, \
			OT_MANAGED_PROPERTY_GETTER_CALLBACK_TYPE (___getterStartedCallback) \
		}; \
	___scope: \
		___getterType get##___propertyObjectName(void) ___getterConstDecl { \
			return this->getPropertyManager()->##___propertyGetter(___propertyGroup, ___propertyName); \
		}

//! @def OT_ADD_MANAGED_PROPERTY_SETTERGETTER
//! @brief Add setter and getter methods to access a created property.
//! @note Note that the property must be created (by calling set once) before get will successfully return a value.
//! This can be archieved by either using a registrar or by setting up the properties for a serialized document.
//! @param ___score Scope that will be used to put the methods in (i.e. public, private, protected).
//! @param ___propertyType Type of the property. Must match naming convention of OpenTwin properties.
//! @param ___propertyObjectName Name that will be used for object related objects and methods. (e.g. 'MyVal' will result in 'getMyVal' and so on).
//! @param ___propertyName Name of the property. Will be used to set the name of the created property.
//! @param ___propertyGroup Name of group to store the property at.
//! @param ___setterType Value type that is used for the setter method (e.g. int or const std::string&).
//! @param ___propertySetter Property manager setter method name that will be called to set the value of a property (see: ot::PropertyManager). The set value must match the ___setterType.
//! @param ___getterType Value type that is used for the getter method (e.g. int or const std::string&).
//! @param ___propertyGetter Property manager getter method name that will be called to get the value of a property (see: ot::PropertyManager). The returned value must match the ___getterType.
//! @param ___getterConstDecl Will be added to the end of the getter method. Should be either kept empty or const.
//! @param ___getterStartedCallback Will be called at the beginning of the getter method before accessing the property.
//! @param ___setterCompletedCallback Will be called at the end of the setter method after the property was created/updated.
//! @param __VA_ARGS__ Initializer list for property.
//! @ref OT_ADD_MANAGED_PROPERTY_SETTER
//! @ref OT_ADD_MANAGED_PROPERTY_GETTER
//! @ref ot::PropertyManager
//! @ref ot::ManagedPropertyRegistrar
#define OT_ADD_MANAGED_PROPERTY_SETTERGETTER(___scope, ___propertyType, ___propertyObjectName, ___propertyGroup, ___propertyName, ___setterType, ___propertySetter, ___getterType, ___propertyGetter, ___getterConstDecl, ___setterCompletedCallback, ___getterStartedCallback, ...) \
	OT_ADD_MANAGED_PROPERTY_SETTER(___scope, ___propertyType, ___propertyObjectName, ___propertyGroup, ___propertyName, ___setterType, ___propertySetter, ___setterCompletedCallback, __VA_ARGS__) \
	OT_ADD_MANAGED_PROPERTY_GETTER(___scope, ___propertyType, ___propertyObjectName, ___propertyGroup, ___propertyName, ___getterType, ___propertyGetter, ___getterConstDecl, ___getterStartedCallback, __VA_ARGS__)

//! @brief OT_DECL_MANAGED_PROPERTY
//! @brief Declare a property.
//! Creates a registrar and the getter and setter methods.
//! @param ___score Scope that will be used to put the methods in (i.e. public, private, protected).
//! @param ___propertyType Type of the property. Must match naming convention of OpenTwin properties.
//! @param ___propertyObjectName Name that will be used for object related objects and methods. (e.g. 'MyVal' will result in 'getMyVal' and so on).
//! @param ___propertyName Name of the property. Will be used to set the name of the created property.
//! @param ___propertyGroup Name of group to store the property at.
//! @param ___setterType Value type that is used for the setter method (e.g. int or const std::string&).
//! @param ___propertySetter Property manager setter method name that will be called to set the value of a property (see: ot::PropertyManager). The set value must match the ___setterType.
//! @param ___getterType Value type that is used for the getter method (e.g. int or const std::string&).
//! @param ___propertyGetter Property manager getter method name that will be called to get the value of a property (see: ot::PropertyManager). The returned value must match the ___getterType.
//! @param ___getterConstDecl Will be added to the end of the getter method. Should be either kept empty or const.
//! @ref OT_CREATE_AND_REGISTER_MANAGED_PROPERTY
//! @ref OT_ADD_MANAGED_PROPERTY_SETTERGETTER
#define OT_DECL_MANAGED_PROPERTY(___scope, ___propertyType, ___propertyObjectName, ___propertyGroup, ___propertyName, ___setterType, ___propertySetter, ___getterType, ___propertyGetter, ___getterConstDecl, ...) \
	OT_CREATE_AND_REGISTER_MANAGED_PROPERTY(___propertyType, ___propertyObjectName, ___propertyGroup, ___propertyName, __VA_ARGS__); \
	OT_ADD_MANAGED_PROPERTY_SETTERGETTER(___scope, ___propertyType, ___propertyObjectName, ___propertyGroup, ___propertyName, ___setterType, ___propertySetter, ___getterType, ___propertyGetter, ___getterConstDecl, , , __VA_ARGS__)

//! @brief Declare a property and add callbacks to the get and set methods.
//! Creates a registrar and the getter and setter methods.
//! Passes the provided callback methods to enable notifications for property changes.
//! @param ___score Scope that will be used to put the methods in (i.e. public, private, protected).
//! @param ___propertyType Type of the property. Must match naming convention of OpenTwin properties.
//! @param ___propertyObjectName Name that will be used for object related objects and methods. (e.g. 'MyVal' will result in 'getMyVal' and so on).
//! @param ___propertyName Name of the property. Will be used to set the name of the created property.
//! @param ___propertyGroup Name of group to store the property at.
//! @param ___setterType Value type that is used for the setter method (e.g. int or const std::string&).
//! @param ___propertySetter Property manager setter method name that will be called to set the value of a property (see: ot::PropertyManager). The set value must match the ___setterType.
//! @param ___getterType Value type that is used for the getter method (e.g. int or const std::string&).
//! @param ___propertyGetter Property manager getter method name that will be called to get the value of a property (see: ot::PropertyManager). The returned value must match the ___getterType.
//! @param ___getterConstDecl Will be added to the end of the getter method. Should be either kept empty or const.
//! @param ___getterStartedCallback Pass lambda here that will be called at the beginning of the getter method before accessing the property.
//! @param ___setterCompletedCallback Pass lambda here that will be called at the end of the setter method after the property was created/updated.
//! @ref OT_CREATE_AND_REGISTER_MANAGED_PROPERTY
//! @ref OT_ADD_MANAGED_PROPERTY_SETTERGETTER
#define OT_DECL_MANAGED_PROPERTY_CALL(___scope, ___propertyType, ___propertyObjectName, ___propertyGroup, ___propertyName, ___setterType, ___propertySetter, ___getterType, ___propertyGetter, ___getterConstDecl, ___setterCompletedCallback, ___getterStartedCallback, ...) \
	OT_CREATE_AND_REGISTER_MANAGED_PROPERTY(___propertyType, ___propertyObjectName, ___propertyGroup, ___propertyName, __VA_ARGS__); \
	OT_ADD_MANAGED_PROPERTY_SETTERGETTER(___scope, ___propertyType, ___propertyObjectName, ___propertyGroup, ___propertyName, ___setterType, ___propertySetter, ___getterType, ___propertyGetter, ___getterConstDecl, ___setterCompletedCallback, ___getterStartedCallback, __VA_ARGS__)

//! @def OT_DECL_BASIC_MANAGED_PROPERTY
//! @brief Declare a basic property.
//! A basic property has a const getter, and the property setter/getter have the OpenTwin format (e.g setBool and getBool).
//! @param ___score Scope that will be used to put the methods in (i.e. public, private, protected).
//! @param ___propertyType Type of the property. Must match naming convention of OpenTwin properties.
//! @param ___propertyObjectName Name that will be used for object related objects and methods. (e.g. 'MyVal' will result in 'getMyVal' and so on).
//! @param ___propertyName Name of the property. Will be used to set the name of the created property.
//! @param ___propertyGroup Name of group to store the property at.
//! @param ___setterType Value type that is used for the setter method (e.g. int or const std::string&).
//! @param ___propertySetter Property manager setter method name that will be called to set the value of a property (see: ot::PropertyManager). The set value must match the ___setterType.
//! @param ___getterType Value type that is used for the getter method (e.g. int or const std::string&).
//! @param ___propertyGetter Property manager getter method name that will be called to get the value of a property (see: ot::PropertyManager). The returned value must match the ___getterType.
//! @param ___getterConstDecl Will be added to the end of the getter method. Should be either kept empty or const.
//! @param ___setterCompletedCallback Pass lambda here that will be called at the end of the setter method after the property was created/updated.
//! @param ___getterStartedCallback Pass lambda here that will be called at the beginning of the getter method before accessing the property.
//! @ref OT_DECL_MANAGED_PROPERTY
#define OT_DECL_BASIC_MANAGED_PROPERTY(___scope, ___baseType, ___propertyType, ___propertyObjectName, ___propertyGroup, ___propertyName, ...) \
	OT_DECL_MANAGED_PROPERTY(___scope, ___propertyType, ___propertyObjectName, ___propertyGroup, ___propertyName, ___baseType, set##___propertyType, ___baseType, get##___propertyType, const, __VA_ARGS__)

//! @def OT_DECL_BASIC_MANAGED_PROPERTY_CALL
//! @brief Declare a basic property.
//! A basic property has a const getter, and the property setter/getter have the OpenTwin format (e.g setBool and getBool).
//! @ref OT_DECL_MANAGED_PROPERTY_CALL
#define OT_DECL_BASIC_MANAGED_PROPERTY_CALL(___scope, ___baseType, ___propertyType, ___propertyObjectName, ___propertyGroup, ___propertyName, ___setterCompletedCallback, ___getterStartedCallback, ...) \
	OT_DECL_MANAGED_PROPERTY_CALL(___scope, ___propertyType, ___propertyObjectName, ___propertyGroup, ___propertyName, ___baseType, set##___propertyType, ___baseType, get##___propertyType, const, ___setterCompletedCallback, ___getterStartedCallback, __VA_ARGS__)

//! @def OT_DECL_MANAGED_REF_PROPERTY
//! @brief Declare a basic reference property.
//! A basic reference property has a const getter, and the property setter/getter have the OpenTwin format (e.g setString and getString).
//! The setter and getter use const references.
//! @ref OT_DECL_MANAGED_PROPERTY
#define OT_DECL_MANAGED_REF_PROPERTY(___scope, ___baseType, ___propertyType, ___propertyObjectName, ___propertyGroup, ___propertyName, ___propertySetter, ___propertyGetter, ...) \
	OT_DECL_MANAGED_PROPERTY(___scope, ___propertyType, ___propertyObjectName, ___propertyGroup, ___propertyName, const ___baseType&, ___propertySetter, const ___baseType&, ___propertyGetter, const, __VA_ARGS__)

//! @def OT_DECL_MANAGED_REF_PROPERTY_CALL
//! @brief Declare a basic reference property.
//! A basic reference property has a const getter, and the property setter/getter have the OpenTwin format (e.g setString and getString).
//! The setter and getter use const references.
//! @ref OT_DECL_MANAGED_PROPERTY_CALL
#define OT_DECL_MANAGED_REF_PROPERTY_CALL(___scope, ___baseType, ___propertyType, ___propertyObjectName, ___propertyGroup, ___propertyName, ___propertySetter, ___propertyGetter, ___setterCompletedCallback, ___getterStartedCallback, ...) \
	OT_DECL_MANAGED_PROPERTY_CALL(___scope, ___propertyType, ___propertyObjectName, ___propertyGroup, ___propertyName, const ___baseType&, ___propertySetter, const ___baseType&, ___propertyGetter, const, ___setterCompletedCallback, ___getterStartedCallback, __VA_ARGS__)

//! @def OT_DECL_MANAGED_PTR_PROPERTY
//! @brief Declare a basic pointer property.
//! A basic pointer property has a const getter, and the property setter/getter have the OpenTwin format (e.g setPainter2D and getPainter2D).
//! The getter uses a const and setter a regular pointer.
//! @ref OT_DECL_MANAGED_PROPERTY
#define OT_DECL_MANAGED_PTR_PROPERTY(___scope, ___baseType, ___propertyType, ___propertyObjectName, ___propertyGroup, ___propertyName, ___propertySetter, ___propertyGetter, ...) \
	OT_DECL_MANAGED_PROPERTY(___scope, ___propertyType, ___propertyObjectName, ___propertyGroup, ___propertyName, ___baseType*, ___propertySetter, const ___baseType*, ___propertyGetter, const, __VA_ARGS__)

//! @def OT_DECL_MANAGED_PTR_PROPERTY_CALL
//! @brief Declare a basic pointer property.
//! A basic pointer property has a const getter, and the property setter/getter have the OpenTwin format (e.g setPainter2D and getPainter2D).
//! The getter uses a const and setter a regular pointer.
//! @ref OT_DECL_MANAGED_PROPERTY_CALL
#define OT_DECL_MANAGED_PTR_PROPERTY_CALL(___scope, ___baseType, ___propertyType, ___propertyObjectName, ___propertyGroup, ___propertyName, ___propertySetter, ___propertyGetter, ___setterCompletedCallback, ___getterStartedCallback, ...) \
	OT_DECL_MANAGED_PROPERTY_CALL(___scope, ___propertyType, ___propertyObjectName, ___propertyGroup, ___propertyName, ___baseType*, ___propertySetter, const ___baseType*, ___propertyGetter, const, ___setterCompletedCallback, ___getterStartedCallback, __VA_ARGS__)

//! @def OT_DECL_MANAGED_GENERAL_PROPERTY
//! @brief Declare a general property.
//! A general property has public setter and getter methods and uses the property group "General" to store the property.
//! @ref OT_DECL_MANAGED_PROPERTY
#define OT_DECL_MANAGED_GENERAL_PROPERTY(___baseType, ___propertyType, ___propertyObjectName, ___propertyGroup, ___propertyName, ___setterType, ___propertySetter, ___getterType, ___propertyGetter, ___getterConstDecl, ...) \
	OT_DECL_MANAGED_PROPERTY(public, ___baseType, ___propertyType, ___propertyObjectName, ___propertyGroup, ___propertyName, ___setterType, ___propertySetter, ___getterType, ___propertyGetter, ___getterConstDecl, __VA_ARGS__)

//! @def OT_DECL_MANAGED_GENERAL_PROPERTY_CALL
//! @brief Declare a general property.
//! A general property has public setter and getter methods and uses the property group "General" to store the property.
//! @ref OT_DECL_MANAGED_PROPERTY_CALL
#define OT_DECL_MANAGED_GENERAL_PROPERTY_CALL(___baseType, ___propertyType, ___propertyObjectName, ___propertyGroup, ___propertyName, ___setterType, ___propertySetter, ___getterType, ___propertyGetter, ___getterConstDecl, ___setterCompletedCallback, ___getterStartedCallback, ...) \
	OT_DECL_MANAGED_PROPERTY_CALL(public, ___baseType, ___propertyType, ___propertyObjectName, ___propertyGroup, ___propertyName, ___setterType, ___propertySetter, ___getterType, ___propertyGetter, ___getterConstDecl, ___setterCompletedCallback, ___getterStartedCallback, __VA_ARGS__)

//! @def OT_DECL_MANAGED_GENERAL_BASIC_PROPERTY
//! @brief Declare a general basic property.
//! This is in extension of the basic property mixing it with the general property.
//! A general property has public setter and getter methods and uses the property group "General" to store the property.
//! A basic property has a const getter, and the property setter/getter have the OpenTwin format (e.g setBool and getBool).
//! @ref OT_DECL_BASIC_MANAGED_PROPERTY
#define OT_DECL_MANAGED_GENERAL_BASIC_PROPERTY(___baseType, ___propertyType, ___propertyObjectName, ___propertyGroup, ___propertyName, ...) OT_DECL_BASIC_MANAGED_PROPERTY(public, ___baseType, ___propertyType, ___propertyObjectName, ___propertyGroup, ___propertyName, __VA_ARGS__)

//! @def OT_DECL_MANAGED_GENERAL_BASIC_PROPERTY_CALL
//! @brief Declare a general basic property.
//! This is in extension of the basic property mixing it with the general property.
//! A general property has public setter and getter methods and uses the property group "General" to store the property.
//! A basic property has a const getter, and the property setter/getter have the OpenTwin format (e.g setBool and getBool).
//! @ref OT_DECL_BASIC_MANAGED_PROPERTY_CALL
#define OT_DECL_MANAGED_GENERAL_BASIC_PROPERTY_CALL(___baseType, ___propertyType, ___propertyObjectName, ___propertyGroup, ___propertyName, ___setterCompletedCallback, ___getterStartedCallback, ...) OT_DECL_BASIC_MANAGED_PROPERTY_CALL(public, ___baseType, ___propertyType, ___propertyObjectName, ___propertyGroup, ___propertyName, ___setterCompletedCallback, ___getterStartedCallback, __VA_ARGS__)

//! @def OT_DECL_MANAGED_GENERAL_REF_PROPERTY
//! @brief Declare a general basic reference property.
//! This is in extension of the basic reference property mixing it with the general property.
//! A general property has public setter and getter methods and uses the property group "General" to store the property.
//! A basic reference property has a const getter, and the property setter/getter have the OpenTwin format (e.g setString and getString).
//! @ref OT_DECL_MANAGED_REF_PROPERTY
#define OT_DECL_MANAGED_GENERAL_REF_PROPERTY(___baseType, ___propertyType, ___propertyObjectName, ___propertyGroup, ___propertyName, ...) OT_DECL_MANAGED_REF_PROPERTY(public, ___baseType, ___propertyType, ___propertyObjectName, ___propertyGroup, ___propertyName, set##___propertyType, get##___propertyType, __VA_ARGS__)

//! @def OT_DECL_MANAGED_GENERAL_REF_PROPERTY_CALL
//! @brief Declare a general basic reference property.
//! This is in extension of the basic reference property mixing it with the general property.
//! A general property has public setter and getter methods and uses the property group "General" to store the property.
//! A basic reference property has a const getter, and the property setter/getter have the OpenTwin format (e.g setString and getString).
//! @ref OT_DECL_MANAGED_REF_PROPERTY_CALL
#define OT_DECL_MANAGED_GENERAL_REF_PROPERTY_CALL(___baseType, ___propertyType, ___propertyObjectName, ___propertyGroup, ___propertyName, ___setterCompletedCallback, ___getterStartedCallback, ...) OT_DECL_MANAGED_REF_PROPERTY_CALL(public, ___baseType, ___propertyType, ___propertyObjectName, ___propertyGroup, ___propertyName, set##___propertyType, get##___propertyType, ___setterCompletedCallback, ___getterStartedCallback, __VA_ARGS__)

//! @def OT_DECL_MANAGED_GENERAL_PTR_PROPERTY
//! @brief Declare a general basic reference property.
//! This is in extension of the basic reference property mixing it with the general property.
//! A general property has public setter and getter methods and uses the property group "General" to store the property.
//! A basic pointer property has a const getter, and the property setter/getter have the OpenTwin format (e.g setPainter2D and getPainter2D).
//! @ref OT_DECL_MANAGED_PTR_PROPERTY
#define OT_DECL_MANAGED_GENERAL_PTR_PROPERTY(___baseType, ___propertyType, ___propertyObjectName, ___propertyGroup, ___propertyName, ...) OT_DECL_MANAGED_PTR_PROPERTY(public, ___baseType, ___propertyType, ___propertyObjectName, ___propertyGroup, ___propertyName, set##___propertyType, get##___propertyType, __VA_ARGS__)

//! @def OT_DECL_MANAGED_GENERAL_PTR_PROPERTY_CALL
//! @brief Declare a general basic reference property.
//! This is in extension of the basic reference property mixing it with the general property.
//! A general property has public setter and getter methods and uses the property group "General" to store the property.
//! A basic pointer property has a const getter, and the property setter/getter have the OpenTwin format (e.g setPainter2D and getPainter2D).
//! @ref OT_DECL_MANAGED_PTR_PROPERTY_CALL
#define OT_DECL_MANAGED_GENERAL_PTR_PROPERTY_CALL(___baseType, ___propertyType, ___propertyObjectName, ___propertyGroup, ___propertyName, ___setterCompletedCallback, ___getterStartedCallback, ...) OT_DECL_MANAGED_PTR_PROPERTY_CALL(public, ___baseType, ___propertyType, ___propertyObjectName, ___propertyGroup, ___propertyName, set##___propertyType, get##___propertyType, ___setterCompletedCallback, ___getterStartedCallback, __VA_ARGS__)

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// Basic property interface

//! @def OT_MANAGED_PROPERTY_BOOL
//! @brief Declare a boolean property.
//! Will register the property at the objects PropertyManager.
//! Use in the following way: <br>
//! 
//!		class MyClass : public ManagedPropertyObject {
//!			// This will add the getMyProperty and setMyPropertyMethods.
//!			// The created property will be initialized with false.
//!			OT_MANAGED_PROPERTY_BOOL(MyProperty, "General", "My Property", false);
//!		};
//! 
//! @note The object using this macro must inherit from ManagedPropertyObject.
#define OT_MANAGED_PROPERTY_BOOL(___propertyObjectName, ___propertyGroup, ___propertyName, ...) OT_DECL_MANAGED_GENERAL_BASIC_PROPERTY(bool, Bool, ___propertyObjectName, ___propertyGroup, ___propertyName, __VA_ARGS__)

//! @def OT_MANAGED_PROPERTY_BOOL_CALL
//! @brief Declare a boolean property.
//! Will register the property at the objects PropertyManager.
//! Lambdas for getter and setter method can be provided.
//! Use in the following way: <br>
//! 
//!		class MyClass : public ManagedPropertyObject {
//!			// This will add the getMyProperty and setMyPropertyMethods.
//!			// The created property will be initialized with false.
//!			// The getter will remain default.
//!			// The setter will call the virtual method myPropertyHasChanged.
//!			OT_MANAGED_PROPERTY_BOOL_CALL(MyProperty, "General", "My Property", (void), [=](const ot::PropertyBool* _property){
//!				this->myPropertyHasChanged(_property->getValue());
//!			}, false);
//! 
//!		protected:
//!			// Will be called whenever MyProperty was written.
//!			virtual void myPropertyHasChanged(bool _newValue) {};
//!		};
//! 
//! @note The object using this macro must inherit from ManagedPropertyObject.
//! @ref OT_ADD_MANAGED_PROPERTY_SETTERGETTER
//! @param ___propertyObjectName, ___propertyGroup, ___propertyName
#define OT_MANAGED_PROPERTY_BOOL_CALL(___propertyObjectName, ___propertyGroup, ___propertyName, ___setterCompletedCallback, ___getterStartedCallback, ...) OT_DECL_MANAGED_GENERAL_BASIC_PROPERTY_CALL(bool, Bool, ___propertyObjectName, ___propertyGroup, ___propertyName, ___setterCompletedCallback, ___getterStartedCallback, __VA_ARGS__)

//! @def OT_MANAGED_PROPERTY_INT
//! @brief Declare a boolean property.
//! Will register the property at the objects PropertyManager.
//! Use in the following way: <br>
//! 
//!		class MyClass : public ManagedPropertyObject {
//!			// This will add the getMyProperty and setMyPropertyMethods.
//!			// The created property will be initialized with 0.
//!			OT_MANAGED_PROPERTY_INT(MyProperty, "General", "My Property", 0);
//!		};
//! 
//! @note The object using this macro must inherit from ManagedPropertyObject.
//! @ref OT_DECL_MANAGED_GENERAL_BASIC_PROPERTY
#define OT_MANAGED_PROPERTY_INT(___propertyObjectName, ___propertyGroup, ___propertyName, ...) OT_DECL_MANAGED_GENERAL_BASIC_PROPERTY(int, Int, ___propertyObjectName, ___propertyGroup, ___propertyName, __VA_ARGS__)

//! @def OT_MANAGED_PROPERTY_INT_CALL
//! @brief Declare a integer property.
//! Will register the property at the objects PropertyManager.
//! Lambdas for getter and setter method can be provided.
//! Use in the following way: <br>
//! 
//!		class MyClass : public ManagedPropertyObject {
//!			// This will add the getMyProperty and setMyPropertyMethods.
//!			// The created property will be initialized with 0.
//!			// The getter will remain default.
//!			// The setter will call the virtual method myPropertyHasChanged.
//!			OT_MANAGED_PROPERTY_INT_CALL(MyProperty, "General", "My Property", (void), [=](const ot::PropertyInt* _property){
//!				this->myPropertyHasChanged(_property->getValue());
//!			}, 0);
//! 
//!		protected:
//!			// Will be called whenever MyProperty was written.
//!			virtual void myPropertyHasChanged(int _newValue) {};
//!		};
//! 
//! @note The object using this macro must inherit from ManagedPropertyObject.
//! @ref OT_DECL_MANAGED_GENERAL_BASIC_PROPERTY_CALL
#define OT_MANAGED_PROPERTY_INT_CALL(___propertyObjectName, ___propertyGroup, ___propertyName, ___setterCompletedCallback, ___getterStartedCallback, ...) OT_DECL_MANAGED_GENERAL_BASIC_PROPERTY_CALL(int, Int, ___propertyObjectName, ___propertyGroup, ___propertyName, ___setterCompletedCallback, ___getterStartedCallback, __VA_ARGS__)

//! @def OT_MANAGED_PROPERTY_DOUBLE
//! @brief Declare a double property.
//! Will register the property at the objects PropertyManager.
//! Use in the following way: <br>
//! 
//!		class MyClass : public ManagedPropertyObject {
//!			// This will add the getMyProperty and setMyPropertyMethods.
//!			// The created property will be initialized with 0.
//!			OT_MANAGED_PROPERTY_DOUBLE(MyProperty, "General", "My Property", 0.0);
//!		};
//! 
//! @note The object using this macro must inherit from ManagedPropertyObject.
//! @ref OT_DECL_MANAGED_GENERAL_BASIC_PROPERTY
#define OT_MANAGED_PROPERTY_DOUBLE(___propertyObjectName, ___propertyGroup, ___propertyName, ...) OT_DECL_MANAGED_GENERAL_BASIC_PROPERTY(double, Double, ___propertyObjectName, ___propertyGroup, ___propertyName, __VA_ARGS__)

//! @def OT_MANAGED_PROPERTY_DOUBLE_CALL
//! @brief Declare a double property.
//! Will register the property at the objects PropertyManager.
//! Lambdas for getter and setter method can be provided.
//! Use in the following way: <br>
//! 
//!		class MyClass : public ManagedPropertyObject {
//!			// This will add the getMyProperty and setMyPropertyMethods.
//!			// The created property will be initialized with 0.
//!			// The getter will remain default.
//!			// The setter will call the virtual method myPropertyHasChanged.
//!			OT_MANAGED_PROPERTY_DOUBLE_CALL(MyProperty, "General", "My Property", (void), [=](const ot::PropertyDouble* _property){
//!				this->myPropertyHasChanged(_property->getValue());
//!			}, 0.0);
//! 
//!		protected:
//!			// Will be called whenever MyProperty was written.
//!			virtual void myPropertyHasChanged(double _newValue) {};
//!		};
//! 
//! @note The object using this macro must inherit from ManagedPropertyObject.
//! @ref OT_DECL_MANAGED_GENERAL_BASIC_PROPERTY_CALL
#define OT_MANAGED_PROPERTY_DOUBLE_CALL(___propertyObjectName, ___propertyGroup, ___propertyName, ___setterCompletedCallback, ___getterStartedCallback, ...) OT_DECL_MANAGED_GENERAL_BASIC_PROPERTY_CALL(double, Double, ___propertyObjectName, ___propertyGroup, ___propertyName, ___setterCompletedCallback, ___getterStartedCallback, __VA_ARGS__)

//! @def OT_MANAGED_PROPERTY_STRING
//! @brief Declare a string property.
//! Will register the property at the objects PropertyManager.
//! Use in the following way: <br>
//! 
//!		class MyClass : public ManagedPropertyObject {
//!			// This will add the getMyProperty and setMyPropertyMethods.
//!			// The created property will be initialized with an empty string.
//!			OT_MANAGED_PROPERTY_STRING(MyProperty, "General", "My Property", "");
//!		};
//! 
//! @note The object using this macro must inherit from ManagedPropertyObject.
//! @ref OT_DECL_MANAGED_GENERAL_REF_PROPERTY
#define OT_MANAGED_PROPERTY_STRING(___propertyObjectName, ___propertyGroup, ___propertyName, ...) OT_DECL_MANAGED_GENERAL_REF_PROPERTY(std::string, String, ___propertyObjectName, ___propertyGroup, ___propertyName, __VA_ARGS__)

//! @def OT_MANAGED_PROPERTY_STRING_CALL
//! @brief Declare a boolean property.
//! Will register the property at the objects PropertyManager.
//! Lambdas for getter and setter method can be provided.
//! Use in the following way: <br>
//! 
//!		class MyClass : public ManagedPropertyObject {
//!			// This will add the getMyProperty and setMyPropertyMethods.
//!			// The created property will be initialized with false.
//!			// The getter will remain default.
//!			// The setter will call the virtual method myPropertyHasChanged.
//!			OT_MANAGED_PROPERTY_STRING_CALL(MyProperty, "General", "My Property", (void), [=](const ot::PropertyString* _property){
//!				this->myPropertyHasChanged(_property->getValue());
//!			}, false);
//! 
//!		protected:
//!			// Will be called whenever MyProperty was written.
//!			virtual void myPropertyHasChanged(const std::string& _newValue) {};
//!		};
//! 
//! @note The object using this macro must inherit from ManagedPropertyObject.
//! @ref OT_DECL_MANAGED_GENERAL_REF_PROPERTY_CALL
#define OT_MANAGED_PROPERTY_STRING_CALL(___propertyObjectName, ___propertyGroup, ___propertyName, ___setterCompletedCallback, ___getterStartedCallback, ...) OT_DECL_MANAGED_GENERAL_REF_PROPERTY_CALL(std::string, String, ___propertyObjectName, ___propertyGroup, ___propertyName, ___setterCompletedCallback, ___getterStartedCallback, __VA_ARGS__)

//! @def OT_MANAGED_PROPERTY_COLOR
//! @brief Declare a color property.
//! Will register the property at the objects PropertyManager.
//! Use in the following way: <br>
//! 
//!		class MyClass : public ManagedPropertyObject {
//!			// This will add the getMyProperty and setMyPropertyMethods.
//!			// The created property will be initialized with the color red.
//!			OT_MANAGED_PROPERTY_COLOR(MyProperty, "General", "My Property", ot::Color(ot::Red));
//!		};
//! 
//! @note The object using this macro must inherit from ManagedPropertyObject.
//! @ref OT_DECL_MANAGED_GENERAL_REF_PROPERTY
#define OT_MANAGED_PROPERTY_COLOR(___propertyObjectName, ___propertyGroup, ___propertyName, ...) OT_DECL_MANAGED_GENERAL_REF_PROPERTY(ot::Color, Color, ___propertyObjectName, ___propertyGroup, ___propertyName, __VA_ARGS__)

//! @def OT_MANAGED_PROPERTY_COLOR_CALL
//! @brief Declare a color property.
//! Will register the property at the objects PropertyManager.
//! Lambdas for getter and setter method can be provided.
//! Use in the following way: <br>
//! 
//!		class MyClass : public ManagedPropertyObject {
//!			// This will add the getMyProperty and setMyPropertyMethods.
//!			// The created property will be initialized with the color red.
//!			// The getter will remain default.
//!			// The setter will call the virtual method myPropertyHasChanged.
//!			OT_MANAGED_PROPERTY_COLOR_CALL(MyProperty, "General", "My Property", (void), [=](const ot::PropertyColor* _property){
//!				this->myPropertyHasChanged(_property->getValue());
//!			}, ot::Color(ot::Red));
//! 
//!		protected:
//!			// Will be called whenever MyProperty was written.
//!			virtual void myPropertyHasChanged(const ot::Color& _newValue) {};
//!		};
//! 
//! @note The object using this macro must inherit from ManagedPropertyObject.
//! @ref OT_DECL_MANAGED_GENERAL_REF_PROPERTY_CALL
#define OT_MANAGED_PROPERTY_COLOR_CALL(___propertyObjectName, ___propertyGroup, ___propertyName, ___setterCompletedCallback, ___getterStartedCallback, ...) OT_DECL_MANAGED_GENERAL_REF_PROPERTY_CALL(ot::Color, Color, ___propertyObjectName, ___propertyGroup, ___propertyName, ___setterCompletedCallback, ___getterStartedCallback, __VA_ARGS__)

//! @def OT_MANAGED_PROPERTY_PAINTER2D
//! @brief Declare a painter 2D property.
//! Will register the property at the objects PropertyManager.
//! Use in the following way: <br>
//! 
//!		class MyClass : public ManagedPropertyObject {
//!			// This will add the getMyProperty and setMyPropertyMethods.
//!			// The created property will be initialized with a black fill painter.
//!			OT_MANAGED_PROPERTY_PAINTER2D(MyProperty, "General", "My Property", new ot::FillPainter(ot::Black));
//!		};
//! 
//! @note The object using this macro must inherit from ManagedPropertyObject.
//! @ref OT_DECL_MANAGED_GENERAL_PTR_PROPERTY
#define OT_MANAGED_PROPERTY_PAINTER2D(___propertyObjectName, ___propertyGroup, ___propertyName, ...) OT_DECL_MANAGED_GENERAL_PTR_PROPERTY(ot::Painter2D, Painter2D, ___propertyObjectName, ___propertyGroup, ___propertyName, __VA_ARGS__)

//! @def OT_MANAGED_PROPERTY_PAINTER2D_CALL
//! @brief Declare a painter 2D property.
//! Will register the property at the objects PropertyManager.
//! Lambdas for getter and setter method can be provided.
//! Use in the following way: <br>
//! 
//!		class MyClass : public ManagedPropertyObject {
//!			// This will add the getMyProperty and setMyPropertyMethods.
//!			// The created property will be initialized with a black fill painter.
//!			// The getter will remain default.
//!			// The setter will call the virtual method myPropertyHasChanged.
//!			OT_MANAGED_PROPERTY_PAINTER2D_CALL(MyProperty, "General", "My Property", (void), [=](const ot::PropertyPainter2D* _property) {
//!				this->myPropertyHasChanged(_property->getPainter());
//!			}, new ot::FillPainter(ot::Black));
//! 
//!		protected:
//!			// Will be called whenever MyProperty was written.
//!			virtual void myPropertyHasChanged(const ot::Painter2D* _newPainter) {};
//!		};
//! 
//! @note The object using this macro must inherit from ManagedPropertyObject.
//! @ref OT_DECL_MANAGED_GENERAL_PTR_PROPERTY_CALL
#define OT_MANAGED_PROPERTY_PAINTER2D_CALL(___propertyObjectName, ___propertyGroup, ___propertyName, ___setterCompletedCallback, ___getterStartedCallback, ...) OT_DECL_MANAGED_GENERAL_PTR_PROPERTY_CALL(ot::Painter2D, Painter2D, ___propertyObjectName, ___propertyGroup, ___propertyName, ___setterCompletedCallback, ___getterStartedCallback, __VA_ARGS__)

//! @def OT_MANAGED_PROPERTY_PATH
//! @brief Declare a path property.
//! Will register the property at the objects PropertyManager.
//! Use in the following way: <br>
//! 
//!		class MyClass : public ManagedPropertyObject {
//!			// This will add the getMyProperty and setMyPropertyMethods.
//!			// The created property will be initialized with an empty string.
//!			OT_MANAGED_PROPERTY_PATH(MyProperty, "General", "My Property", "");
//!		};
//! 
//! @note The object using this macro must inherit from ManagedPropertyObject.
//! @ref OT_DECL_MANAGED_GENERAL_REF_PROPERTY.
#define OT_MANAGED_PROPERTY_PATH(___propertyObjectName, ___propertyGroup, ___propertyName, ...) OT_DECL_MANAGED_GENERAL_REF_PROPERTY(std::string, Path, ___propertyObjectName, ___propertyGroup, ___propertyName, __VA_ARGS__)

//! @def OT_MANAGED_PROPERTY_PATH_CALL
//! @brief Declare a path property.
//! Will register the property at the objects PropertyManager.
//! Lambdas for getter and setter method can be provided.
//! Use in the following way: <br>
//! 
//!		class MyClass : public ManagedPropertyObject {
//!			// This will add the getMyProperty and setMyPropertyMethods.
//!			// The created property will be initialized with an empty string.
//!			// The getter will remain default.
//!			// The setter will call the virtual method myPropertyHasChanged.
//!			OT_MANAGED_PROPERTY_PATH_CALL(MyProperty, "General", "My Property", (void), [=](const ot::PropertyPath* _property){
//!				this->myPropertyHasChanged(_property->getPath());
//!			}, false);
//! 
//!		protected:
//!			// Will be called whenever MyProperty was written.
//!			virtual void myPropertyHasChanged(const std::string& _newValue) {};
//!		};
//! 
//! @note The object using this macro must inherit from ManagedPropertyObject.
//! @ref OT_DECL_MANAGED_GENERAL_REF_PROPERTY_CALL
#define OT_MANAGED_PROPERTY_PATH_CALL(___propertyObjectName, ___propertyGroup, ___propertyName, ___setterCompletedCallback, ___getterStartedCallback, ...) OT_DECL_MANAGED_GENERAL_REF_PROPERTY_CALL(std::string, Path, ___propertyObjectName, ___propertyGroup, ___propertyName, ___setterCompletedCallback, ___getterStartedCallback, __VA_ARGS__)

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// Convenience properties

//! @def OT_ADD_MANAGED_BASIC_COMBO_PROPERTY_SETTERGETTER_2
//! @brief Add setter and getter methods for an object that is based on two basic properties
#define OT_ADD_MANAGED_BASIC_COMBO_PROPERTY_SETTERGETTER_2(___propertyObjectName, ___propertyGroup, ___valueType, ___propertyBaseTypeName, ___propertyObjectName_1, ___propertyGetter_1, ___propertyObjectName_2, ___propertyGetter_2) \
	public: \
		void set##___propertyObjectName(const ___valueType& _value) { \
			this->getPropertyManager()->set##___propertyBaseTypeName( \
				___propertyGroup, \
				___propertyObjectName_1, \
				_value.##___propertyGetter_1() \
			); \
			this->getPropertyManager()->set##___propertyBaseTypeName( \
				___propertyGroup, \
				___propertyObjectName_2, \
				_value.##___propertyGetter_2() \
			); \
		} \
		___valueType get##___propertyObjectName(void) const { \
			return ___valueType( \
				this->getPropertyManager()->get##___propertyBaseTypeName( \
					___propertyGroup, \
					___propertyObjectName_1 \
				), \
				this->getPropertyManager()->get##___propertyBaseTypeName( \
					___propertyGroup, \
					___propertyObjectName_2 \
				) \
			); \
		}

//! @def OT_MANAGED_PROPERTY_POINT2D
//! @brief Declare a Point2D property.
//! Creates two integer properties.
//! Will register the properties at the objects PropertyManager.
//! Use in the following way: <br>
//! 
//!		class MyClass : public ManagedPropertyObject {
//!			// This will add the getMyProperty and setMyPropertyMethods.
//!			// The created property will be initialized with 0.
//!			OT_MANAGED_PROPERTY_POINT2D(MyProperty, "General", "My Property", 0, 0);
//!		};
//! 
//! The generated object names for the base type will get the ("X"; "Y") suffix.
//! The generated property names for the base type will get the (" X"; " Y") suffix.
//! @note The object using this macro must inherit from ManagedPropertyObject.
//! @ref OT_MANAGED_PROPERTY_INT
//! @ref OT_ADD_MANAGED_BASIC_COMBO_PROPERTY_SETTERGETTER_2
#define OT_MANAGED_PROPERTY_POINT2D(___propertyObjectName, ___propertyGroup, ___propertyName, ___x, ___y) \
	OT_MANAGED_PROPERTY_INT(___propertyObjectName##X, ___propertyGroup, ___propertyName + std::string(" X"), ___x) \
	OT_MANAGED_PROPERTY_INT(___propertyObjectName##Y, ___propertyGroup, ___propertyName + std::string(" Y"), ___y) \
	OT_ADD_MANAGED_BASIC_COMBO_PROPERTY_SETTERGETTER_2(___propertyObjectName, ___propertyGroup, ot::Point2D, Int, ___propertyName + std::string(" X"), x, ___propertyName + std::string(" Y"), y)

//! @def OT_MANAGED_PROPERTY_POINT2D_CALL
//! @brief Declare a Point2D property.
//! Creates two double properties.
//! Will register the properties at the objects PropertyManager.
//! Lambdas for getter and setter method can be provided.
//! Use in the following way: <br>
//! 
//!		class MyClass : public ManagedPropertyObject {
//!			// This will add the getMyProperty and setMyPropertyMethods.
//!			// The created property will be initialized with the color red.
//!			// The getter will remain default.
//!			// The setter will call the virtual method myPropertyHasChanged.
//!			OT_MANAGED_PROPERTY_POINT2D_CALL(MyProperty, "General", "My Property", (void), [=](const ot::PropertyInt* _property){
//!				// Since the Point2D is not a base type for properties 
//!				// the macro will create different base properties.
//!				if (_property->getPropertyName() == "My Property X") {
//!					this->myWidthPropertyHasChanged(_property->getValue());
//!				}
//!				else {
//!					this->myHeightPropertyHasChanged(_property->getValue());
//!				}
//!			}, 0, 0);
//! 
//!		protected:
//!			virtual void myXPropertyHasChanged(int _newValue) {};
//!			virtual void myYPropertyHasChanged(int _newValue) {};
//!		};
//! 
//! The generated object names for the base type will get the ("Width"; "Name") suffix.
//! The generated property names for the base type will get the (" Width"; " Name") suffix.
//! @note The object using this macro must inherit from ManagedPropertyObject.
//! @ref OT_MANAGED_PROPERTY_DOUBLE_CALL
//! @ref OT_ADD_MANAGED_BASIC_COMBO_PROPERTY_SETTERGETTER_2
#define OT_MANAGED_PROPERTY_POINT2D_CALL(___propertyObjectName, ___propertyGroup, ___propertyName, ___setterCompletedCallback, ___getterStartedCallback, ___x, ___y) \
	OT_MANAGED_PROPERTY_INT_CALL(___propertyObjectName##X, ___propertyGroup, ___propertyName + std::string(" X"), ___setterCompletedCallback, ___getterStartedCallback, ___x) \
	OT_MANAGED_PROPERTY_INT_CALL(___propertyObjectName##Y, ___propertyGroup, ___propertyName + std::string(" Y"), ___setterCompletedCallback, ___getterStartedCallback, ___y) \
	OT_ADD_MANAGED_BASIC_COMBO_PROPERTY_SETTERGETTER_2(___propertyObjectName, ___propertyGroup, ot::Point2D, Int, ___propertyName + std::string(" X"), x, ___propertyName + std::string(" Y"), y)

//! @def OT_MANAGED_PROPERTY_POINT2D
//! @brief Declare a Point2D property.
//! Creates two double properties.
//! Will register the properties at the objects PropertyManager.
//! Use in the following way: <br>
//! 
//!		class MyClass : public ManagedPropertyObject {
//!			// This will add the getMyProperty and setMyPropertyMethods.
//!			// The created property will be initialized with 0.
//!			OT_MANAGED_PROPERTY_POINT2D(MyProperty, "General", "My Property", 0, 0);
//!		};
//! 
//! The generated object names for the base type will get the ("X"; "Y") suffix.
//! The generated property names for the base type will get the (" X"; " Y") suffix.
//! @note The object using this macro must inherit from ManagedPropertyObject.
//! @ref OT_MANAGED_PROPERTY_DOUBLE
//! @ref OT_ADD_MANAGED_BASIC_COMBO_PROPERTY_SETTERGETTER_2
#define OT_MANAGED_PROPERTY_POINT2DD(___propertyObjectName, ___propertyGroup, ___propertyName, ___x, ___y) \
	OT_MANAGED_PROPERTY_DOUBLE(___propertyObjectName##X, ___propertyGroup, ___propertyName + std::string(" X"), ___x) \
	OT_MANAGED_PROPERTY_DOUBLE(___propertyObjectName##Y, ___propertyGroup, ___propertyName + std::string(" Y"), ___y) \
	OT_ADD_MANAGED_BASIC_COMBO_PROPERTY_SETTERGETTER_2(___propertyObjectName, ___propertyGroup, ot::Point2DD, Double, ___propertyName + std::string(" X"), x, ___propertyName + std::string(" Y"), y)

//! @def OT_MANAGED_PROPERTY_POINT2DD_CALL
//! @brief Declare a Point2DD property.
//! Creates two double properties.
//! Will register the properties at the objects PropertyManager.
//! Lambdas for getter and setter method can be provided.
//! Use in the following way: <br>
//! 
//!		class MyClass : public ManagedPropertyObject {
//!			// This will add the getMyProperty and setMyPropertyMethods.
//!			// The created property will be initialized with the color red.
//!			// The getter will remain default.
//!			// The setter will call the virtual method myPropertyHasChanged.
//!			OT_MANAGED_PROPERTY_POINT2DD_CALL(MyProperty, "General", "My Property", (void), [=](const ot::PropertyDouble* _property){
//!				// Since the Point2DD is not a base type for properties 
//!				// the macro will create different base properties.
//!				if (_property->getPropertyName() == "My Property X") {
//!					this->myWidthPropertyHasChanged(_property->getValue());
//!				}
//!				else {
//!					this->myHeightPropertyHasChanged(_property->getValue());
//!				}
//!			}, 0.0, 0.0);
//! 
//!		protected:
//!			virtual void myXPropertyHasChanged(double _newValue) {};
//!			virtual void myYPropertyHasChanged(double _newValue) {};
//!		};
//! 
//! The generated object names for the base type will get the ("Width"; "Name") suffix.
//! The generated property names for the base type will get the (" Width"; " Name") suffix.
//! @note The object using this macro must inherit from ManagedPropertyObject.
//! @ref OT_MANAGED_PROPERTY_DOUBLE_CALL
//! @ref OT_ADD_MANAGED_BASIC_COMBO_PROPERTY_SETTERGETTER_2
#define OT_MANAGED_PROPERTY_POINT2DD_CALL(___propertyObjectName, ___propertyGroup, ___propertyName, ___setterCompletedCallback, ___getterStartedCallback, ___x, ___y) \
	OT_MANAGED_PROPERTY_DOUBLE_CALL(___propertyObjectName##X, ___propertyGroup, ___propertyName + std::string(" X"), ___setterCompletedCallback, ___getterStartedCallback, ___x) \
	OT_MANAGED_PROPERTY_DOUBLE_CALL(___propertyObjectName##Y, ___propertyGroup, ___propertyName + std::string(" Y"), ___setterCompletedCallback, ___getterStartedCallback, ___y) \
	OT_ADD_MANAGED_BASIC_COMBO_PROPERTY_SETTERGETTER_2(___propertyObjectName, ___propertyGroup, ot::Point2DD, Double, ___propertyName + std::string(" X"), x, ___propertyName + std::string(" Y"), y)

//! @def OT_MANAGED_PROPERTY_SIZE2D
//! @brief Declare a Size2D property.
//! Creates two integer properties.
//! Will register the properties at the objects PropertyManager.
//! Use in the following way: <br>
//! 
//!		class MyClass : public ManagedPropertyObject {
//!			// This will add the getMyProperty and setMyPropertyMethods.
//!			// The created property will be initialized with 0.
//!			OT_MANAGED_PROPERTY_SIZE2D(MyProperty, "General", "My Property", 0, 0);
//!		};
//! 
//! The generated object names for the base type will get the ("Width"; "Name") suffix.
//! The generated property names for the base type will get the (" Width"; " Name") suffix.
//! @note The object using this macro must inherit from ManagedPropertyObject.
//! @ref OT_MANAGED_PROPERTY_INT
//! @ref OT_ADD_MANAGED_BASIC_COMBO_PROPERTY_SETTERGETTER_2
#define OT_MANAGED_PROPERTY_SIZE2D(___propertyObjectName, ___propertyGroup, ___propertyName, ___width, ___height) \
	OT_MANAGED_PROPERTY_INT(___propertyObjectName##Width, ___propertyGroup, ___propertyName + std::string(" Width"), ___width) \
	OT_MANAGED_PROPERTY_INT(___propertyObjectName##Height, ___propertyGroup, ___propertyName + std::string(" Height"), ___height) \
	OT_ADD_MANAGED_BASIC_COMBO_PROPERTY_SETTERGETTER_2(___propertyObjectName, ___propertyGroup, ot::Size2D, Int, ___propertyName + std::string(" Width"), width, ___propertyName + std::string(" Height"), height)

//! @def OT_MANAGED_PROPERTY_SIZE2D_CALL
//! @brief Declare a Size2D property.
//! Creates two double properties.
//! Will register the properties at the objects PropertyManager.
//! Lambdas for getter and setter method can be provided.
//! Use in the following way: <br>
//! 
//!		class MyClass : public ManagedPropertyObject {
//!			// This will add the getMyProperty and setMyPropertyMethods.
//!			// The created property will be initialized with the color red.
//!			// The getter will remain default.
//!			// The setter will call the virtual method myPropertyHasChanged.
//!			OT_MANAGED_PROPERTY_SIZE2D_CALL(MyProperty, "General", "My Property", (void), [=](const ot::PropertyInt* _property){
//!				// Since the Size2D is not a base type for properties 
//!				// the macro will create different base properties.
//!				if (_property->getPropertyName() == "My Property Width") {
//!					this->myWidthPropertyHasChanged(_property->getValue());
//!				}
//!				else {
//!					this->myHeightPropertyHasChanged(_property->getValue());
//!				}
//!			}, 0, 0);
//! 
//!		protected:
//!			virtual void myWidthPropertyHasChanged(int _newValue) {};
//!			virtual void myHeightPropertyHasChanged(int _newValue) {};
//!		};
//! 
//! The generated object names for the base type will get the ("Width"; "Name") suffix.
//! The generated property names for the base type will get the (" Width"; " Name") suffix.
//! @note The object using this macro must inherit from ManagedPropertyObject.
//! @ref OT_MANAGED_PROPERTY_INT_CALL
//! @ref OT_ADD_MANAGED_BASIC_COMBO_PROPERTY_SETTERGETTER_2
#define OT_MANAGED_PROPERTY_SIZE2D_CALL(___propertyObjectName, ___propertyGroup, ___propertyName, ___setterCompletedCallback, ___getterStartedCallback, ___width, ___height) \
	OT_MANAGED_PROPERTY_INT_CALL(___propertyObjectName##Width, ___propertyGroup, ___propertyName + std::string(" Width"), ___setterCompletedCallback, ___getterStartedCallback, ___width) \
	OT_MANAGED_PROPERTY_INT_CALL(___propertyObjectName##Height, ___propertyGroup, ___propertyName + std::string(" Height"), ___setterCompletedCallback, ___getterStartedCallback, ___height) \
	OT_ADD_MANAGED_BASIC_COMBO_PROPERTY_SETTERGETTER_2(___propertyObjectName, ___propertyGroup, ot::Size2D, Int, ___propertyName + std::string(" Width"), width, ___propertyName + std::string(" Height"), height)

//! @def OT_MANAGED_PROPERTY_SIZE2DD
//! @brief Declare a Size2DD property.
//! Creates two double properties.
//! Will register the properties at the objects PropertyManager.
//! Use in the following way: <br>
//! 
//!		class MyClass : public ManagedPropertyObject {
//!			// This will add the getMyProperty and setMyPropertyMethods.
//!			// The created property will be initialized with 0.
//!			OT_MANAGED_PROPERTY_SIZE2DD(MyProperty, "General", "My Property", 0.0, 0.0);
//!		};
//! 
//! The generated object names for the base type will get the ("Width"; "Name") suffix.
//! The generated property names for the base type will get the (" Width"; " Name") suffix.
//! @note The object using this macro must inherit from ManagedPropertyObject.
//! @ref OT_MANAGED_PROPERTY_DOUBLE
//! @ref OT_ADD_MANAGED_BASIC_COMBO_PROPERTY_SETTERGETTER_2
#define OT_MANAGED_PROPERTY_SIZE2DD(___propertyObjectName, ___propertyGroup, ___propertyName, ___width, ___height) \
	OT_MANAGED_PROPERTY_DOUBLE(___propertyObjectName##Width, ___propertyGroup, ___propertyName + std::string(" Width"), ___width) \
	OT_MANAGED_PROPERTY_DOUBLE(___propertyObjectName##Height, ___propertyGroup, ___propertyName + std::string(" Height"), ___height) \
	OT_ADD_MANAGED_BASIC_COMBO_PROPERTY_SETTERGETTER_2(___propertyObjectName, ___propertyGroup, ot::Size2DD, Double, ___propertyName + std::string(" Width"), width, ___propertyName + std::string(" Height"), height)

//! @def OT_MANAGED_PROPERTY_SIZE2DD
//! @brief Declare a Size2DD property.
//! Creates two double properties.
//! Will register the properties at the objects PropertyManager.
//! Lambdas for getter and setter method can be provided.
//! Use in the following way: <br>
//! 
//!		class MyClass : public ManagedPropertyObject {
//!			// This will add the getMyProperty and setMyPropertyMethods.
//!			// The created property will be initialized with the color red.
//!			// The getter will remain default.
//!			// The setter will call the virtual method myPropertyHasChanged.
//!			OT_MANAGED_PROPERTY_SIZE2DD_CALL(MyProperty, "General", "My Property", (void), [=](const ot::PropertyDouble* _property){
//!				// Since the Size2DD is not a base type for properties 
//!				// the macro will create different base properties.
//!				if (_property->getPropertyName() == "My Property Width") {
//!					this->myWidthPropertyHasChanged(_property->getValue());
//!				}
//!				else {
//!					this->myHeightPropertyHasChanged(_property->getValue());
//!				}
//!			}, 0.0, 0.0);
//! 
//!		protected:
//!			virtual void myWidthPropertyHasChanged(double _newValue) {};
//!			virtual void myHeightPropertyHasChanged(double _newValue) {};
//!		};
//! 
//! The generated object names for the base type will get the ("Width"; "Name") suffix.
//! The generated property names for the base type will get the (" Width"; " Name") suffix.
//! @note The object using this macro must inherit from ManagedPropertyObject.
//! @ref OT_MANAGED_PROPERTY_DOUBLE_CALL
//! @ref OT_ADD_MANAGED_BASIC_COMBO_PROPERTY_SETTERGETTER_2
#define OT_MANAGED_PROPERTY_SIZE2DD_CALL(___propertyObjectName, ___propertyGroup, ___propertyName, ___setterCompletedCallback, ___getterStartedCallback, ___width, ___height) \
	OT_MANAGED_PROPERTY_DOUBLE_CALL(___propertyObjectName##Width, ___propertyGroup, ___propertyName + std::string(" Width"), ___setterCompletedCallback, ___getterStartedCallback, ___width) \
	OT_MANAGED_PROPERTY_DOUBLE_CALL(___propertyObjectName##Height, ___propertyGroup, ___propertyName + std::string(" Height"), ___setterCompletedCallback, ___getterStartedCallback, ___height) \
	OT_ADD_MANAGED_BASIC_COMBO_PROPERTY_SETTERGETTER_2(___propertyObjectName, ___propertyGroup, ot::Size2DD, Double, ___propertyName + std::string(" Width"), width, ___propertyName + std::string(" Height"), height)
