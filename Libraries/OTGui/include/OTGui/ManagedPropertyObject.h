//! @file ManagedPropertyObject.h
//! @author Alexander Kuester (alexk95)
//! @date March 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OT header
#include "OTCore/OTAssert.h"
#include "OTGui/PropertyInt.h"
#include "OTGui/PropertyBool.h"
#include "OTGui/FillPainter2D.h"
#include "OTGui/PropertyGroup.h"
#include "OTGui/PropertyColor.h"
#include "OTGui/PropertyDouble.h"
#include "OTGui/PropertyString.h"
#include "OTGui/PropertyManager.h"
#include "OTGui/PropertyFilePath.h"
#include "OTGui/PropertyDirectory.h"
#include "OTGui/PropertyPainter2D.h"
#include "OTGui/PropertyStringList.h"

namespace ot {

	//! @class ManagedPropertyObject
	//! @brief The ManagedPropertyObject class is a basic wrapper for the use of the PropertyManager.
	class OT_CORE_API_EXPORTONLY ManagedPropertyObject {
	public:
		ManagedPropertyObject();
		ManagedPropertyObject(const ManagedPropertyObject& _other);
		ManagedPropertyObject(ManagedPropertyObject&& _other);
		virtual ~ManagedPropertyObject() {};

		ManagedPropertyObject& operator = (const ManagedPropertyObject& _other);
		ManagedPropertyObject& operator = (ManagedPropertyObject&& _other) noexcept;

		//! @brief Serialize the properties into the provided JSON object.
		//! @param _object JSON object to write the properties into.
		//! @param _allocator JSON allocator to use.
		void addPropertiesToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const;

		//! @brief Serialize the properties into the provided JSON object.
		//! @param _object JSON object to read the properties from.
		void setPropertiesFromJsonObject(const ot::ConstJsonObject& _object);

		//! @brief Get the property manager that holds all properties.
		PropertyManager& getPropertyManager(void) { return m_manager; };

		//! @brief Get the property manager that holds all properties.
		const PropertyManager& getPropertyManager(void) const { return m_manager; };

	private:
		PropertyManager m_manager;

	};


	template <class T> class ManagedPropertyRegistrar {
		OT_DECL_NOCOPY(ManagedPropertyRegistrar)
			OT_DECL_NOMOVE(ManagedPropertyRegistrar)
			OT_DECL_NODEFAULT(ManagedPropertyRegistrar)
	public:
		ManagedPropertyRegistrar(ManagedPropertyObject* _object, const std::string& _groupName, ot::Property* _property) {
			OTAssertNullptr(_object);
			OTAssertNullptr(dynamic_cast<T*>(_property));
			_object->getPropertyManager().addProperty(_groupName, _property);
		}
	};

}

//! @def OT_CREATE_AND_REGISTER_PROPERTY
//! @brief Create a property and registr it at the PropertyManager.
//! The registrar will register the property at the ot::PropertyManager upon object creation.
//! The resulting code will have the following structure: <br>
//!  <br>
//! Example with "OT_CREATE_AND_REGISTER_PROPERTY(Bool, MyBool, General, false)": <br>
//! 
//!		private: <br>
//!			ot::ManagedPropertyRegistrar<ot::PropertyBool> m_intern_MyBool {
//!				this,
//!				"General",
//!				new ot::PropertyBool { "MyBool", false }
//!			}
//! @param ___propertyType Type of the property. Must match naming convention of OpenTwin properties.
//! @param ___propertyName Name of the property. Will be used to name the object and will be used to set the name of the created property.
//! @param ___propertyGroup Name of group to store the property at.
//! @param __VA_ARGS__ Initializer list for property.
#define OT_CREATE_AND_REGISTER_PROPERTY(___propertyType, ___propertyName, ___propertyGroup, ...) \
	private: ot::ManagedPropertyRegistrar<ot::Property##___propertyType> m_intern_##___propertyName { \
		this, \
		#___propertyGroup, \
		new ot::Property##___propertyType{ #___propertyName, __VA_ARGS__ } \
	}

//! @def OT_ADD_PROPERTY_SETTER
//! @brief Add setter method to access a created property.
//! @param ___score Scope that will be used to put the methods in (i.e. public, private, protected).
//! @param ___propertyType Type of the property. Must match naming convention of OpenTwin properties.
//! @param ___propertyName Name of the property. Will be used to name the object and will be used to set the name of the created property.
//! @param ___propertyGroup Name of group to store the property at.
//! @param ___setterType Value type that is used for the setter method (e.g. int or const std::string&).
//! @param ___propertySetter Property manager setter method name that will be called to set the value of a property (see: ot::PropertyManager). The set value must match the ___setterType.
//! @param ___setterCompletedCallback Will be called at the end of the setter method after the property was created/updated.
//! @param __VA_ARGS__ Initializer list for property.
//! @ref OT_ADD_PROPERTY_GETTER
//! @ref OT_ADD_PROPERTY_SETTERGETTER
//! @ref ot::PropertyManager
//! @ref ot::ManagedPropertyRegistrar
#define OT_ADD_PROPERTY_SETTER(___scope, ___propertyType, ___propertyName, ___propertyGroup, ___setterType, ___propertySetter, ___setterCompletedCallback, ...) \
	___scope: void set##___propertyName(___setterType _value) { \
		auto result = this->getPropertyManager().##___propertySetter(#___propertyGroup, #___propertyName, _value); \
		___setterCompletedCallback(result); \
}

//! @def OT_ADD_PROPERTY_GETTER
//! @brief Add getter method to access a created property.
//! @note Note that the property must be created (by calling set once) before get will successfully return a value.
//! This can be archieved by either using a registrar or by setting up the properties for a serialized document.
//! @param ___score Scope that will be used to put the methods in (i.e. public, private, protected).
//! @param ___propertyType Type of the property. Must match naming convention of OpenTwin properties.
//! @param ___propertyName Name of the property. Will be used to name the object and will be used to set the name of the created property.
//! @param ___propertyGroup Name of group to store the property at.
//! @param ___getterType Value type that is used for the getter method (e.g. int or const std::string&).
//! @param ___propertyGetter Property manager getter method name that will be called to get the value of a property (see: ot::PropertyManager). The returned value must match the ___getterType.
//! @param ___getterConstDecl Will be added to the end of the getter method. Should be either kept empty or const.
//! @param ___getterStartedCallback Will be called at the beginning of the getter method before accessing the property.
//! @param __VA_ARGS__ Initializer list for property.
//! @ref OT_ADD_PROPERTY_SETTER
//! @ref OT_ADD_PROPERTY_SETTERGETTER
//! @ref ot::PropertyManager
//! @ref ot::ManagedPropertyRegistrar
#define OT_ADD_PROPERTY_GETTER(___scope, ___propertyType, ___propertyName, ___propertyGroup, ___getterType, ___propertyGetter, ___getterConstDecl, ___getterStartedCallback, ...) \
	___scope: ___getterType get##___propertyName(void) ___getterConstDecl { \
		___getterConstDecl auto& manager = this->getPropertyManager(); \
		___getterStartedCallback(manager); \
		return manager.##___propertyGetter(#___propertyGroup, #___propertyName); \
	}

//! @def OT_ADD_PROPERTY_SETTERGETTER
//! @brief Add setter and getter methods to access a created property.
//! @note Note that the property must be created (by calling set once) before get will successfully return a value.
//! This can be archieved by either using a registrar or by setting up the properties for a serialized document.
//! @param ___score Scope that will be used to put the methods in (i.e. public, private, protected).
//! @param ___propertyType Type of the property. Must match naming convention of OpenTwin properties.
//! @param ___propertyName Name of the property. Will be used to name the object and will be used to set the name of the created property.
//! @param ___propertyGroup Name of group to store the property at.
//! @param ___setterType Value type that is used for the setter method (e.g. int or const std::string&).
//! @param ___propertySetter Property manager setter method name that will be called to set the value of a property (see: ot::PropertyManager). The set value must match the ___setterType.
//! @param ___getterType Value type that is used for the getter method (e.g. int or const std::string&).
//! @param ___propertyGetter Property manager getter method name that will be called to get the value of a property (see: ot::PropertyManager). The returned value must match the ___getterType.
//! @param ___getterConstDecl Will be added to the end of the getter method. Should be either kept empty or const.
//! @param ___getterStartedCallback Will be called at the beginning of the getter method before accessing the property.
//! @param ___setterCompletedCallback Will be called at the end of the setter method after the property was created/updated.
//! @param __VA_ARGS__ Initializer list for property.
//! @ref OT_ADD_PROPERTY_SETTER
//! @ref OT_ADD_PROPERTY_GETTER
//! @ref ot::PropertyManager
//! @ref ot::ManagedPropertyRegistrar
#define OT_ADD_PROPERTY_SETTERGETTER(___scope, ___propertyType, ___propertyName, ___propertyGroup, ___setterType, ___propertySetter, ___getterType, ___propertyGetter, ___getterConstDecl, ___getterStartedCallback, ___setterCompletedCallback, ...) \
	OT_ADD_PROPERTY_SETTER(___scope, ___propertyType, ___propertyName, ___propertyGroup, ___setterType, ___propertySetter, ___setterCompletedCallback, __VA_ARGS__) \
	OT_ADD_PROPERTY_GETTER(___scope, ___propertyType, ___propertyName, ___propertyGroup, ___getterType, ___propertyGetter, ___getterConstDecl, ___getterStartedCallback, __VA_ARGS__)

//! @brief Declare a property.
//! Creates a registrar and the getter and setter methods.
//! @param ___score Scope that will be used to put the methods in (i.e. public, private, protected).
//! @param ___propertyType Type of the property. Must match naming convention of OpenTwin properties.
//! @param ___propertyName Name of the property. Will be used to name the object and will be used to set the name of the created property.
//! @param ___propertyGroup Name of group to store the property at.
//! @param ___setterType Value type that is used for the setter method (e.g. int or const std::string&).
//! @param ___propertySetter Property manager setter method name that will be called to set the value of a property (see: ot::PropertyManager). The set value must match the ___setterType.
//! @param ___getterType Value type that is used for the getter method (e.g. int or const std::string&).
//! @param ___propertyGetter Property manager getter method name that will be called to get the value of a property (see: ot::PropertyManager). The returned value must match the ___getterType.
//! @param ___getterConstDecl Will be added to the end of the getter method. Should be either kept empty or const.
//! @ref OT_CREATE_AND_REGISTER_PROPERTY
//! @ref OT_ADD_PROPERTY_SETTERGETTER
#define OT_DECL_PROPERTY(___scope, ___baseType, ___propertyType, ___propertyName, ___propertyGroup, ___setterType, ___propertySetter, ___getterType, ___propertyGetter, ___getterConstDecl, ...) \
	OT_CREATE_AND_REGISTER_PROPERTY(___propertyType, ___propertyName, ___propertyGroup, __VA_ARGS__); \
	OT_ADD_PROPERTY_SETTERGETTER(___scope, ___propertyType, ___propertyName, ___propertyGroup, ___setterType, ___propertySetter, ___getterType, ___propertyGetter, ___getterConstDecl, [](){}, [](){}, __VA_ARGS__)

//! @brief Declare a property and add callbacks to the get and set methods.
//! Creates a registrar and the getter and setter methods.
//! Passes the provided callback methods to enable notifications for property changes.
//! @param ___score Scope that will be used to put the methods in (i.e. public, private, protected).
//! @param ___propertyType Type of the property. Must match naming convention of OpenTwin properties.
//! @param ___propertyName Name of the property. Will be used to name the object and will be used to set the name of the created property.
//! @param ___propertyGroup Name of group to store the property at.
//! @param ___setterType Value type that is used for the setter method (e.g. int or const std::string&).
//! @param ___propertySetter Property manager setter method name that will be called to set the value of a property (see: ot::PropertyManager). The set value must match the ___setterType.
//! @param ___getterType Value type that is used for the getter method (e.g. int or const std::string&).
//! @param ___propertyGetter Property manager getter method name that will be called to get the value of a property (see: ot::PropertyManager). The returned value must match the ___getterType.
//! @param ___getterConstDecl Will be added to the end of the getter method. Should be either kept empty or const.
//! @param ___getterStartedCallback Pass lambda here that will be called at the beginning of the getter method before accessing the property.
//! @param ___setterCompletedCallback Pass lambda here that will be called at the end of the setter method after the property was created/updated.
//! @ref OT_CREATE_AND_REGISTER_PROPERTY
//! @ref OT_ADD_PROPERTY_SETTERGETTER
#define OT_DECL_PROPERTY_CALL(___scope, ___baseType, ___propertyType, ___propertyName, ___propertyGroup, ___setterType, ___propertySetter, ___getterType, ___propertyGetter, ___getterConstDecl, ___getterStartedCallback, ___setterCompletedCallback, ...) \
	OT_CREATE_AND_REGISTER_PROPERTY(___propertyType, ___propertyName, ___propertyGroup, __VA_ARGS__); \
	OT_ADD_PROPERTY_SETTERGETTER(___scope, ___propertyType, ___propertyName, ___propertyGroup, ___setterType, ___propertySetter, ___getterType, ___propertyGetter, ___getterConstDecl, ___getterStartedCallback, ___setterCompletedCallback, __VA_ARGS__)

//! @def OT_DECL_BASIC_PROPERTY
//! @brief Declare a basic property.
#define OT_DECL_BASIC_PROPERTY(___scope, ___baseType, ___propertyType, ___propertyName, ___propertyGroup, ...) \
	OT_DECL_PROPERTY(___scope, ___baseType, ___propertyType, ___propertyName, ___propertyGroup, ___baseType, set##___propertyType, ___baseType, get##___propertyType, const, __VA_ARGS__)

//! @def OT_DECL_BASIC_PROPERTY_CALL
#define OT_DECL_BASIC_PROPERTY_CALL(___scope, ___baseType, ___propertyType, ___propertyName, ___propertyGroup, ___getterStartedCallback, ___setterCompletedCallback, ...) \
	OT_DECL_PROPERTY_CALL(___scope, ___baseType, ___propertyType, ___propertyName, ___propertyGroup, ___baseType, set##___propertyType, ___baseType, get##___propertyType, const, ___getterStartedCallback, ___setterCompletedCallback, __VA_ARGS__)

//! @def OT_DECL_REF_PROPERTY
//! @brief Declare a reference property.
#define OT_DECL_REF_PROPERTY(___scope, ___baseType, ___propertyType, ___propertyName, ___propertyGroup, ___propertySetter, ___propertyGetter, ...) \
	OT_DECL_PROPERTY(___scope, ___baseType, ___propertyType, ___propertyName, ___propertyGroup, const ___baseType&, ___propertySetter, const ___baseType&, ___propertyGetter, const, __VA_ARGS__)

//! @def OT_DECL_REF_PROPERTY_CALL
#define OT_DECL_REF_PROPERTY_CALL(___scope, ___baseType, ___propertyType, ___propertyName, ___propertyGroup, ___propertySetter, ___propertyGetter, ___getterStartedCallback, ___setterCompletedCallback, ...) \
	OT_DECL_PROPERTY_CALL(___scope, ___baseType, ___propertyType, ___propertyName, ___propertyGroup, const ___baseType&, ___propertySetter, const ___baseType&, ___propertyGetter, const, ___getterStartedCallback, ___setterCompletedCallback, __VA_ARGS__)

//! @def OT_DECL_PTR_PROPERTY
//! @brief Declare a pointer property.
#define OT_DECL_PTR_PROPERTY(___scope, ___baseType, ___propertyType, ___propertyName, ___propertyGroup, ___propertySetter, ___propertyGetter, ...) \
	OT_DECL_PROPERTY(___scope, ___baseType, ___propertyType, ___propertyName, ___propertyGroup, ___baseType*, ___propertySetter, const ___baseType*, ___propertyGetter, const, __VA_ARGS__)

//! @def OT_DECL_PTR_PROPERTY_CALL
#define OT_DECL_PTR_PROPERTY_CALL(___scope, ___baseType, ___propertyType, ___propertyName, ___propertyGroup, ___propertySetter, ___propertyGetter, ___getterStartedCallback, ___setterCompletedCallback, ...) \
	OT_DECL_PROPERTY_CALL(___scope, ___baseType, ___propertyType, ___propertyName, ___propertyGroup, ___baseType*, ___propertySetter, const ___baseType*, ___propertyGetter, const, ___getterStartedCallback, ___setterCompletedCallback, __VA_ARGS__)

//! @def OT_DECL_GENERAL_PROPERTY
#define OT_DECL_GENERAL_PROPERTY(___baseType, ___propertyType, ___propertyName, ___setterType, ___propertySetter, ___getterType, ___propertyGetter, ___getterConstDecl, ...) \
	OT_DECL_PROPERTY(public, ___baseType, ___propertyType, ___propertyName, General, ___setterType, ___propertySetter, ___getterType, ___propertyGetter, ___getterConstDecl, __VA_ARGS__)

//! @def OT_DECL_GENERAL_PROPERTY_CALL
#define OT_DECL_GENERAL_PROPERTY_CALL(___baseType, ___propertyType, ___propertyName, ___setterType, ___propertySetter, ___getterType, ___propertyGetter, ___getterConstDecl, ___getterStartedCallback, ___setterCompletedCallback, ...) \
	OT_DECL_PROPERTY_CALL(public, ___baseType, ___propertyType, ___propertyName, General, ___setterType, ___propertySetter, ___getterType, ___propertyGetter, ___getterConstDecl, ___getterStartedCallback, ___setterCompletedCallback, __VA_ARGS__)

//! @def OT_DECL_GENERAL_BASIC_PROPERTY
#define OT_DECL_GENERAL_BASIC_PROPERTY(___baseType, ___propertyType, ___propertyName, ...) OT_DECL_BASIC_PROPERTY(public, ___baseType, ___propertyType, ___propertyName, General, __VA_ARGS__)

//! @def OT_DECL_GENERAL_BASIC_PROPERTY_CALL
#define OT_DECL_GENERAL_BASIC_PROPERTY_CALL(___baseType, ___propertyType, ___propertyName, ___getterStartedCallback, ___setterCompletedCallback, ...) OT_DECL_BASIC_PROPERTY_CALL(public, ___baseType, ___propertyType, ___propertyName, General, ___getterStartedCallback, ___setterCompletedCallback, __VA_ARGS__)

//! @def OT_DECL_GENERAL_REF_PROPERTY
#define OT_DECL_GENERAL_REF_PROPERTY(___baseType, ___propertyType, ___propertyName, ...) OT_DECL_REF_PROPERTY(public, ___baseType, ___propertyType, ___propertyName, General, set##___propertyType, get##___propertyType, __VA_ARGS__)

//! @def OT_DECL_GENERAL_REF_PROPERTY_CALL
#define OT_DECL_GENERAL_REF_PROPERTY_CALL(___baseType, ___propertyType, ___propertyName, ___getterStartedCallback, ___setterCompletedCallback, ...) OT_DECL_REF_PROPERTY_CALL(public, ___baseType, ___propertyType, ___propertyName, General, set##___propertyType, get##___propertyType, ___getterStartedCallback, ___setterCompletedCallback, __VA_ARGS__)

//! @def OT_DECL_GENERAL_PTR_PROPERTY
#define OT_DECL_GENERAL_PTR_PROPERTY(___baseType, ___propertyType, ___propertyName, ...) OT_DECL_PTR_PROPERTY(public, ___baseType, ___propertyType, ___propertyName, General, set##___propertyType, get##___propertyType, __VA_ARGS__)

//! @def OT_DECL_GENERAL_PTR_PROPERTY_CALL
#define OT_DECL_GENERAL_PTR_PROPERTY_CALL(___baseType, ___propertyType, ___propertyName, ___getterStartedCallback, ___setterCompletedCallback, ...) OT_DECL_PTR_PROPERTY_CALL(public, ___baseType, ___propertyType, ___propertyName, General, set##___propertyType, get##___propertyType, ___getterStartedCallback, ___setterCompletedCallback, __VA_ARGS__)

//! @def OT_PROPERTY_BOOL
//! @brief Declare a boolean property.
#define OT_PROPERTY_BOOL(___propertyName, ...) OT_DECL_GENERAL_BASIC_PROPERTY(bool, Bool, ___propertyName, __VA_ARGS__)

//! @def OT_PROPERTY_BOOL_CALL
//! @brief Declare a boolean property.
//! Lambdas for getter and setter method conde injection can be provided.
//! @ref OT_ADD_PROPERTY_SETTERGETTER
//! @param ___propertyName
#define OT_PROPERTY_BOOL_CALL(___propertyName, ___getterStartedCallback, ___setterCompletedCallback, ...) OT_DECL_GENERAL_BASIC_PROPERTY_CALL(bool, Bool, ___propertyName, ___getterStartedCallback, ___setterCompletedCallback, __VA_ARGS__)

//! @def OT_PROPERTY_INT
//! @brief Declare an integer property.
#define OT_PROPERTY_INT(___propertyName, ...) OT_DECL_GENERAL_BASIC_PROPERTY(int, Int, ___propertyName, __VA_ARGS__)

//! @def OT_PROPERTY_INT_CALL
#define OT_PROPERTY_INT_CALL(___propertyName, ___getterStartedCallback, ___setterCompletedCallback, ...) OT_DECL_GENERAL_BASIC_PROPERTY_CALL(int, Int, ___propertyName, ___getterStartedCallback, ___setterCompletedCallback, __VA_ARGS__)

//! @def OT_PROPERTY_DOUBLE
//! @brief Declare a double property.
#define OT_PROPERTY_DOUBLE(___propertyName, ...) OT_DECL_GENERAL_BASIC_PROPERTY(double, Double, ___propertyName, __VA_ARGS__)

//! @def OT_PROPERTY_DOUBLE_CALL
#define OT_PROPERTY_DOUBLE_CALL(___propertyName, ___getterStartedCallback, ___setterCompletedCallback, ...) OT_DECL_GENERAL_BASIC_PROPERTY_CALL(double, Double, ___propertyName, ___getterStartedCallback, ___setterCompletedCallback, __VA_ARGS__)

//! @def OT_PROPERTY_STRING
//! @brief Declare a string property.
#define OT_PROPERTY_STRING(___propertyName, ...) OT_DECL_GENERAL_REF_PROPERTY(std::string, String, ___propertyName, __VA_ARGS__)

//! @def OT_PROPERTY_STRING_CALL
#define OT_PROPERTY_STRING_CALL(___propertyName, ___getterStartedCallback, ___setterCompletedCallback, ...) OT_DECL_GENERAL_REF_PROPERTY_CALL(std::string, String, ___propertyName, ___getterStartedCallback, ___setterCompletedCallback, __VA_ARGS__)

//! @def OT_PROPERTY_COLOR
//! @brief Declare a color property.
#define OT_PROPERTY_COLOR(___propertyName, ...) OT_DECL_GENERAL_REF_PROPERTY(ot::Color, Color, ___propertyName, __VA_ARGS__)

//! @def OT_PROPERTY_COLOR_CALL
#define OT_PROPERTY_COLOR_CALL(___propertyName, ___getterStartedCallback, ___setterCompletedCallback, ...) OT_DECL_GENERAL_REF_PROPERTY_CALL(ot::Color, Color, ___propertyName, ___getterStartedCallback, ___setterCompletedCallback, __VA_ARGS__)

//! @def OT_PROPERTY_PAINTER2D
//! @brief Declare a 2D painter property.
#define OT_PROPERTY_PAINTER2D(___propertyName, ...) OT_DECL_GENERAL_PTR_PROPERTY(ot::Painter2D, Painter2D, ___propertyName, __VA_ARGS__)

//! @def OT_PROPERTY_PAINTER2D_CALL
#define OT_PROPERTY_PAINTER2D_CALL(___propertyName, ___getterStartedCallback, ___setterCompletedCallback, ...) OT_DECL_GENERAL_PTR_PROPERTY_CALL(ot::Painter2D, Painter2D, ___propertyName, ___getterStartedCallback, ___setterCompletedCallback, __VA_ARGS__)

//! @def OT_PROPERTY_FILEPATH
//! @brief Declare a file path property.
#define OT_PROPERTY_FILEPATH(___propertyName, ...) OT_DECL_GENERAL_REF_PROPERTY(std::string, FilePath, ___propertyName, __VA_ARGS__)

//! @def OT_PROPERTY_FILEPATH_CALL
#define OT_PROPERTY_FILEPATH_CALL(___propertyName, ___getterStartedCallback, ___setterCompletedCallback, ...) OT_DECL_GENERAL_REF_PROPERTY_CALL(std::string, FilePath, ___propertyName, ___getterStartedCallback, ___setterCompletedCallback, __VA_ARGS__)

//! @def OT_PROPERTY_DIRECTORYPATH
//! @brief Declare a directory path property.
#define OT_PROPERTY_DIRECTORYPATH(___propertyName, ...) OT_DECL_GENERAL_REF_PROPERTY(std::string, Directory, ___propertyName, __VA_ARGS__)

//! @def OT_PROPERTY_DIRECTORYPATH_CALL
#define OT_PROPERTY_DIRECTORYPATH_CALL(___propertyName, ___getterStartedCallback, ___setterCompletedCallback, ...) OT_DECL_GENERAL_REF_PROPERTY_CALL(std::string, Directory, ___propertyName, ___getterStartedCallback, ___setterCompletedCallback, __VA_ARGS__)
