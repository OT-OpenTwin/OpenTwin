//! @file PropertyManager.h
//! @author Alexander Kuester (alexk95)
//! @date March 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Color.h"
#include "OTCore/Serializable.h"
#include "OTCore/OTClassHelper.h"
#include "OTGui/OTGuiAPIExport.h"
#include "OTGui/PropertyGridCfg.h"
#include "OTGui/PropertyFilePath.h"

// std header
#include <map>
#include <list>

#pragma warning(disable:4251)

namespace ot {

	class Painter2D;
	class PropertyInt;
	class PropertyBool;
	class PropertyGroup;
	class PropertyColor;
	class PropertyDouble;
	class PropertyString;
	class PropertyDirectory;
	class PropertyPainter2D;
	class PropertyStringList;
	class PropertyManagerNotifier;

	//! @brief Manages various property types and serialization.
	//! The PropertyManager provides functions to set, get, and manage different property types.
	//! It also supports serialization to and from JSON objects.
	class OT_GUI_API_EXPORT PropertyManager : public Serializable {
	public:
		// ###########################################################################################################################################################################################################################################################################################################################

		// Constructor

		//! @brief Default constructor.
		PropertyManager();

		//! @brief Copy constructor.
		//! @param _other The PropertyManager to copy from.
		PropertyManager(const PropertyManager& _other);

		//! @brief Move constructor.
		//! @param _other The PropertyManager to move from.
		PropertyManager(PropertyManager&& _other) noexcept;

		//! @brief Destructor.
		virtual ~PropertyManager();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Operators

		//! @brief Copy assignment operator.
		//! @param _other The PropertyManager to copy from.
		//! @return Reference to the updated PropertyManager.
		PropertyManager& operator = (const PropertyManager& _other);

		//! @brief Move assignment operator.
		//! @param _other The PropertyManager to move from.
		//! @return Reference to the updated PropertyManager.
		PropertyManager& operator = (PropertyManager&& _other) noexcept;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Serialization

		//! @brief Serializes the object to a JSON structure.
		//! @param _object The JSON object reference to populate.
		//! @param _allocator The JSON allocator for memory management.
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! @brief Deserializes the object from a JSON structure.
		//! @param _object The JSON object containing the necessary data.
		//! @throw Throws an exception if the provided JSON object is invalid.
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Event handling

		//! @brief Will be called whenever a property has been changed.
		//! @param _property Property that has been changed.
		virtual void propertyChanged(const Property* _property);

		//! @brief Will be called before a property will be read.
		//! @param _propertyGroupName Name of the group where the property should be located at.
		//! @param _propertyName Name of the property. The name should be unique inside a group.
		virtual void propertyRead(const std::string& _propertyGroupName, const std::string& _propertyName);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter

		//! @brief Adds the property to the specified group.
		//! @param _groupName Name of group to add property to.
		//! @param _property Property to add.
		void addProperty(const std::string& _groupName, Property* _property);

		//! @brief Sets boolean property.
		//! @param _groupName The property group name.
		//! @param _valueName The property value name.
		//! @param _value The boolean value to set.
		//! @param _flags Property flags used when visualizing the property.
		//! @return Pointer to the created property.
		PropertyBool* setBool(const std::string& _groupName, const std::string& _valueName, bool _value, const Property::PropertyFlags& _flags = Property::PropertyFlag::NoFlags);

		//! @brief Sets integer property.
		//! @param _groupName The property group name.
		//! @param _valueName The property value name.
		//! @param _value The value to set.
		//! @param _flags Property flags used when visualizing the property.
		//! @return Pointer to the created property.
		PropertyInt* setInt(const std::string& _groupName, const std::string& _valueName, int _value, const Property::PropertyFlags& _flags = Property::PropertyFlag::NoFlags);
		
		//! @brief Sets integer property.
		//! @param _groupName The property group name.
		//! @param _valueName The property value name.
		//! @param _value The value to set.
		//! @param _min The min value to set.
		//! @param _max The max value to set.
		//! @param _flags Property flags used when visualizing the property.
		//! @return Pointer to the created property.
		PropertyInt* setInt(const std::string& _groupName, const std::string& _valueName, int _value, int _min, int _max, const Property::PropertyFlags& _flags = Property::PropertyFlag::NoFlags);

		//! @brief Sets double property.
		//! @param _groupName The property group name.
		//! @param _valueName The property value name.
		//! @param _value The value to set.
		//! @param _flags Property flags used when visualizing the property.
		//! @return Pointer to the created property.
		PropertyDouble* setDouble(const std::string& _groupName, const std::string& _valueName, double _value, const Property::PropertyFlags& _flags = Property::PropertyFlag::NoFlags);

		//! @brief Sets double property.
		//! @param _groupName The property group name.
		//! @param _valueName The property value name.
		//! @param _value The value to set.
		//! @param _min The min value to set.
		//! @param _max The max value to set.
		//! @param _flags Property flags used when visualizing the property.
		//! @return Pointer to the created property.
		PropertyDouble* setDouble(const std::string& _groupName, const std::string& _valueName, double _value, double _min, double _max, const Property::PropertyFlags& _flags = Property::PropertyFlag::NoFlags);

		//! @brief Sets string property.
		//! @param _groupName The property group name.
		//! @param _valueName The property value name.
		//! @param _value The value to set.
		//! @param _flags Property flags used when visualizing the property.
		//! @return Pointer to the created property.
		PropertyString* setString(const std::string& _groupName, const std::string& _valueName, const std::string& _value, const Property::PropertyFlags& _flags = Property::PropertyFlag::NoFlags);

		//! @brief Sets string property.
		//! @param _groupName The property group name.
		//! @param _valueName The property value name.
		//! @param _value The value to set.
		//! @param _flags Property flags used when visualizing the property.
		//! @return Pointer to the created property.
		PropertyStringList* setStringList(const std::string& _groupName, const std::string& _valueName, const std::string& _value, const std::list<std::string>& _possibleValues, const Property::PropertyFlags& _flags = Property::PropertyFlag::NoFlags);

		//! @brief Sets color property.
		//! @param _groupName The property group name.
		//! @param _valueName The property value name.
		//! @param _value The value to set.
		//! @param _flags Property flags used when visualizing the property.
		//! @return Pointer to the created property.
		PropertyColor* setColor(const std::string& _groupName, const std::string& _valueName, const Color& _value, const Property::PropertyFlags& _flags = Property::PropertyFlag::NoFlags);

		//! @brief Sets painter property.
		//! @param _groupName The property group name.
		//! @param _valueName The property value name.
		//! @param _value The value to set.
		//! @param _flags Property flags used when visualizing the property.
		//! @return Pointer to the created property.
		PropertyPainter2D* setPainter2D(const std::string& _groupName, const std::string& _valueName, Painter2D* _painter, const Property::PropertyFlags& _flags = Property::PropertyFlag::NoFlags);

		//! @brief Sets painter property.
		//! @param _groupName The property group name.
		//! @param _valueName The property value name.
		//! @param _value The value to set.
		//! @param _flags Property flags used when visualizing the property.
		//! @return Pointer to the created property.
		PropertyPainter2D* setPainter2D(const std::string& _groupName, const std::string& _valueName, const Painter2D* _painter, const Property::PropertyFlags& _flags = Property::PropertyFlag::NoFlags);

		//! @brief Sets file path property.
		//! @param _groupName The property group name.
		//! @param _valueName The property value name.
		//! @param _path The path to set.
		//! @param _browseMode Browse mode used for file dialog.
		//! @param _flags Property flags used when visualizing the property.
		//! @return Pointer to the created property.
		PropertyFilePath* setFilePath(const std::string& _groupName, const std::string& _valueName, const std::string& _path, PropertyFilePath::BrowseMode _browseMode = PropertyFilePath::ReadFile, const Property::PropertyFlags& _flags = Property::PropertyFlag::NoFlags);

		//! @brief Sets directory path property.
		//! @param _groupName The property group name.
		//! @param _valueName The property value name.
		//! @param _path The path to set.
		//! @param _flags Property flags used when visualizing the property.
		//! @return Pointer to the created property.
		PropertyDirectory* setDirectory(const std::string& _groupName, const std::string& _valueName, const std::string& _path, const Property::PropertyFlags& _flags = Property::PropertyFlag::NoFlags);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Getter

		//! @brief Finds a property by group and name.
		//! @param _groupName The property group name.
		//! @param _valueName The property value name.
		//! @return Pointer to the found Property, or nullptr if not found.
		Property* findProperty(const std::string& _groupName, const std::string& _valueName) const;

		//! @brief Retrieves boolean property value.
		//! @param _groupName The property group name.
		//! @param _valueName The property value name.
		//! @return The boolean value.
		bool getBool(const std::string& _groupName, const std::string& _valueName) const;

		//! @brief Retrieves int property value.
		//! @param _groupName The property group name.
		//! @param _valueName The property value name.
		//! @return The boolean value.
		int getInt(const std::string& _groupName, const std::string& _valueName) const;

		//! @brief Retrieves double property value.
		//! @param _groupName The property group name.
		//! @param _valueName The property value name.
		//! @return The boolean value.
		double getDouble(const std::string& _groupName, const std::string& _valueName) const;

		//! @brief Retrieves string property value.
		//! @param _groupName The property group name.
		//! @param _valueName The property value name.
		//! @return The boolean value.
		const std::string& getString(const std::string& _groupName, const std::string& _valueName) const;

		//! @brief Retrieves color property value.
		//! @param _groupName The property group name.
		//! @param _valueName The property value name.
		//! @return The color value.
		const Color& getColor(const std::string& _groupName, const std::string& _valueName) const;

		//! @brief Retrieves Painter2D property value.
		//! @param _groupName The property group name.
		//! @param _valueName The property value name.
		//! @return The Painter2D value.
		const Painter2D* getPainter2D(const std::string& _groupName, const std::string& _valueName) const;

		//! @brief Retrieves file path property value.
		//! @param _groupName The property group name.
		//! @param _valueName The property value name.
		//! @return The file path.
		const std::string& getFilePath(const std::string& _groupName, const std::string& _valueName) const;

		//! @brief Retrieves Direcotry property value.
		//! @param _groupName The property group name.
		//! @param _valueName The property value name.
		//! @return The Directory path.
		const std::string& getDirectory(const std::string& _groupName, const std::string& _valueName) const;

		//! @brief Finds a property by group and name.
		//! The found property will be casted to the specified type.
		//! @param _groupName The property group name.
		//! @param _valueName The property value name.
		//! @return Pointer to the found Property, or nullptr if not found.
		template <class T> T* getProperty(const std::string& _groupName, const std::string& _valueName) const {
			T* prop = dynamic_cast<T*>(this->findProperty(_groupName, _valueName));
			OTAssertNullptr(prop);
			return prop;
		}

		//! @brief Creates a grid configuration that can be used to serialize or visualize the properties.
		//! @return Grid configuration.
		PropertyGridCfg createGridConfiguration(void) const;

		//! @brief Removes all groups and properties.
		void clear(void);

		//! @brief Register property manager notifier.
		//! @param _notifier Notifier to add.
		void addNotifier(PropertyManagerNotifier* _notifier);

		//! @brief Remove given notifier from this manager.
		//! @param _notifier Notifier to remove.
		//! @param _destroyObject Will destroy the passed notifier no matter if it exists or not.
		//! @return The removed notifier or nullptr if not found or destroyed.
		PropertyManagerNotifier* removeNotifier(PropertyManagerNotifier* _notifier, bool _destroyObject);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Protected

	protected:

		//! @brief Stores a property within a specified group.
		//! @param _groupName The group to store the property in.
		//! @param _property Pointer to the property to store.
		//! @return Pointer to the stored property.
		virtual Property* storeProperty(const std::string& _groupName, Property* _property);

		//! @brief Find an existing group by name.
		//! @param _group The group name.
		//! @return Pointer to the found PropertyGroup, or nullptr if not found.
		PropertyGroup* findGroup(const std::string& _group) const;

		//! @brief Find or create a group by name.
		//! @param _group The group name.
		//! @return Pointer to the found or created PropertyGroup.
		PropertyGroup* findOrCreateGroup(const std::string& _group);

		//! @brief Notify all registered notifiers about the creation of the property.
		//! @param _property Newly created property.
		void notifyPropertyCreated(Property* _property);

		//! @brief Notify all registered notifiers about the deletion of the property.
		//! @param _property Property that will be destroyed after this call.
		void notifyPropertyDestroyed(Property* _property);

		//! @brief Notify all registed notifiers about the value change of the property.
		//! @param _property Changed property.
		void notifyPropertyChanged(const Property* _property);

	private:
		std::map<std::string, PropertyGroup*> m_groups; //! @brief Map containing all groups.
		std::list<PropertyManagerNotifier*> m_notifier; //! @brief Notifier that will receive all property updates.

	};
}