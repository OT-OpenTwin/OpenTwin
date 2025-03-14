//! @file PropertyManager.h
//! @author Alexander Kuester (alexk95)
//! @date March 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Rect.h"
#include "OTCore/Flags.h"
#include "OTCore/Color.h"
#include "OTCore/Size2D.h"
#include "OTCore/Point2D.h"
#include "OTCore/Point3D.h"
#include "OTCore/Serializable.h"
#include "OTCore/OTClassHelper.h"
#include "OTGui/Property.h"
#include "OTGui/OTGuiAPIExport.h"
#include "OTGui/PropertyGridCfg.h"
#include "OTGui/PropertyPath.h"

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
	class PropertyPainter2D;
	class PropertyStringList;
	class PropertyManagerNotifier;
	class PropertyReadCallbackNotifier;
	class PropertyWriteCallbackNotifierBase;

	//! @brief Manages various property types and serialization.
	//! The PropertyManager provides functions to set, get, and manage different property types.
	//! It also supports serialization to and from JSON objects.
	class OT_GUI_API_EXPORT PropertyManager : public Serializable {
		OT_DECL_NOCOPY(PropertyManager)
	public:
		// Constructor

		//! @brief Default constructor.
		PropertyManager();

		//! @brief Deserialize constructor.
		PropertyManager(const ConstJsonObject& _jsonObject);

		//! @brief Move constructor.
		//! @param _other The PropertyManager to move from.
		PropertyManager(PropertyManager&& _other) noexcept;

		//! @brief Destructor.
		virtual ~PropertyManager();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Operators

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
		virtual void readingProperty(const std::string& _propertyGroupName, const std::string& _propertyName) const;

		//! @brief Set the notifications silenced.
		//! If set property access won't trigger any notification.
		void setSilenceNotifications(bool _silence) { m_silenced = _silence; };
		bool getSilenceNotifications(void) const { return m_silenced; };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter

		//! @brief Adds the property to the specified group.
		//! The property should not exist, an existing one will be replaced.
		//! @param _groupName Name of group to add property to.
		//! @param _property Property to add.
		ot::Property* addProperty(const std::string& _groupName, Property* _property);

		//! @brief Updates an existing property or creates a new one.
		//! @param _groupName Name of group to add property to.
		//! @param _property Property to update.
		void updateProperty(const std::string& _groupName, Property* _property);

		//! @brief Merge the properties with the other manager.
		//! @param _other Other property manager to copy values and properties from.
		//! @param _mergeMode Mode to use when merging.
		void mergeWith(const PropertyManager& _other, const PropertyBase::MergeMode& _mergeMode = PropertyBase::MergeValues);

		//! @brief Updates an existing property or creates a new one.
		//! @param _groupName Name of group to add property to.
		//! @param _property Property to update.
		//! @param _deleteProvidedProperty If true the provided _property will be destroyed.
		void updateProperty(const std::string& _groupName, const Property* _property, bool _deleteProvidedProperty);

		//! @brief Sets boolean property.
		//! @param _groupName The property group name.
		//! @param _valueName The property value name.
		//! @param _value The boolean value to set.
		//! @return Pointer to the set property.
		PropertyBool* setBool(const std::string& _groupName, const std::string& _valueName, bool _value);

		//! @brief Sets integer property.
		//! @param _groupName The property group name.
		//! @param _valueName The property value name.
		//! @param _value The value to set.
		//! @return Pointer to the set property.
		PropertyInt* setInt(const std::string& _groupName, const std::string& _valueName, int _value);

		//! @brief Sets double property.
		//! @param _groupName The property group name.
		//! @param _valueName The property value name.
		//! @param _value The value to set.
		//! @return Pointer to the set property.
		PropertyDouble* setDouble(const std::string& _groupName, const std::string& _valueName, double _value);

		//! @brief Sets string property.
		//! @param _groupName The property group name.
		//! @param _valueName The property value name.
		//! @param _value The value to set.
		//! @return Pointer to the set property.
		PropertyString* setString(const std::string& _groupName, const std::string& _valueName, const std::string& _value);

		//! @brief Sets string property.
		//! @param _groupName The property group name.
		//! @param _valueName The property value name.
		//! @param _value The value to set.
		//! @return Pointer to the set property.
		PropertyStringList* setStringList(const std::string& _groupName, const std::string& _valueName, const std::string& _value);

		//! @brief Sets color property.
		//! @param _groupName The property group name.
		//! @param _valueName The property value name.
		//! @param _value The color to set.
		//! @return Pointer to the set property.
		PropertyColor* setColor(const std::string& _groupName, const std::string& _valueName, const Color& _value);

		//! @brief Sets painter property.
		//! @param _groupName The property group name.
		//! @param _valueName The property value name.
		//! @param _painter The painter to set.
		//! @return Pointer to the set property.
		PropertyPainter2D* setPainter2D(const std::string& _groupName, const std::string& _valueName, Painter2D* _painter);

		//! @brief Sets painter property.
		//! @param _groupName The property group name.
		//! @param _valueName The property value name.
		//! @param _painter The painter to create copy from.
		//! @return Pointer to the set property.
		PropertyPainter2D* setPainter2D(const std::string& _groupName, const std::string& _valueName, const Painter2D* _painter);

		//! @brief Sets path property.
		//! @param _groupName The property group name.
		//! @param _valueName The property value name.
		//! @param _path The path to set.
		//! @return Pointer to the set property.
		PropertyPath* setPath(const std::string& _groupName, const std::string& _valueName, const std::string& _path);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Getter

		//! @brief Finds a property by group and name.
		//! @param _groupName The property group name.
		//! @param _valueName The property value name.
		//! @return Pointer to the found Property, or nullptr if not found.
		Property* findProperty(const std::string& _groupName, const std::string& _valueName);

		//! @brief Finds a property by group and name.
		//! @param _groupName The property group name.
		//! @param _valueName The property value name.
		//! @return Pointer to the found Property, or nullptr if not found.
		const Property* findProperty(const std::string& _groupName, const std::string& _valueName) const;

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

		//! @brief Retrieves path property value.
		//! @param _groupName The property group name.
		//! @param _valueName The property value name.
		//! @return The file path.
		const std::string& getPath(const std::string& _groupName, const std::string& _valueName) const;

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

		//! @brief Return data that is managed by this property manager.
		//! @return Const reference to used object name to its PropertyGroup map.
		const std::map<std::string, PropertyGroup*>& getData(void) const { return m_groups; };

		//! @brief Register property manager notifier.
		//! @param _notifier Notifier to add.
		void addNotifier(PropertyManagerNotifier* _notifier);

		//! @brief Remove given notifier from this manager.
		//! @param _notifier Notifier to remove.
		//! @param _destroyObject Will destroy the passed notifier no matter if it exists or not.
		//! @return The removed notifier or nullptr if not found or destroyed.
		PropertyManagerNotifier* removeNotifier(PropertyManagerNotifier* _notifier, bool _destroyObject);

		//! @brief Add read callback notifier.
		//! The read callback notifier will receive read callbacks for
		//! the property under the currently set property path.
		//! The callback method is called before the property read operation starts.
		//! @param _notifier Notifier to add.
		//! @ref PropertyReadCallbackNotifier
		void addReadCallbackNotifier(PropertyReadCallbackNotifier* _notifier);

		void removeReadCallbackNotifier(PropertyReadCallbackNotifier* _notifier);

		//! @brief Add write callback notifier.
		//! The write callback notifier will receive write callbacks for
		//! the property under the currently set property path.
		//! The callback method is called after the property write operation finishes
		//! and before the 
		//! @param _notifier Notifier to add.
		//! @ref PropertyWriteCallbackNotifierBase
		void addWriteCallbackNotifier(PropertyWriteCallbackNotifierBase* _notifier);

		//! @brief Remove write callback notifier.
		//! @param _notifier Notifier to remove.
		//! @ref PropertyWriteCallbackNotifierBase
		void removeWriteCallbackNotifier(PropertyWriteCallbackNotifierBase* _notifier);

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

	private:
		bool m_silenced; //! @brief If true any property access won't trigger a notification.
		std::map<std::string, PropertyGroup*> m_groups; //! @brief Map containing all groups.
		std::list<PropertyManagerNotifier*> m_notifier; //! @brief Notifier that will receive all property updates.
		std::map<std::string, PropertyReadCallbackNotifier*> m_readNotifier;
		std::map<std::string, PropertyWriteCallbackNotifierBase*> m_writeNotifier;
	};
}
