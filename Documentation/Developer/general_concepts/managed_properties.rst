Managed Properties
##################

OpenTwin provides a convenience layer to ease the property handling.
When developing a library, application or service that does not require frontend functionallity the ``OTGui``
class `Managed Property Object <../_static/codedochtml/classot_1_1_managed_property_object.xhtml>`_
provides a simple interface to add properties that will automatically be concidered for serialization.

Frontend components that include the ``OTWidgets`` library can use the
`Managed Property Widget Object <../_static/codedochtml/classot_1_1_managed_property_widget_object.xhtml>`_
class to enable the use of multiple property visualization related :ref:`property linking wrappers <property_linking>`.

Property Management structure
*****************************

.. image:: images/managed_properties_class.svg

Property Creation
*****************

Adding properties to a class requires a ``OTGui`` `Property Manager <../_static/codedochtml/classot_1_1_property_manager.xhtml>`_ object.
The manager provides the setter and getter methods for all supported properties.

Property Callbacks
==================

When creating a property a callback function for the read and one for the write access to the property can be added.
This callback methods will be called before a property read operation and after a property write operation.
By using the callback methods automatic data dependent updates can be triggered.

.. _tree_entit_name_example:

Tree Entity Name Example
------------------------

The following :ref:`example <tree_entit_name_example_code>` will demonstrate how to create a string property and synchronize it with a tree item.

Convenience macros provided in the `OTGui/ManagedPropertyRegistrarBase.h file<../_static/codedochtml/classot_1_1_managed_property_registrar_base.xhtml>`_ will be used in this :ref:`example <tree_entit_name_example_code>`.

The ``OT_PROPERTY_STRING_CALL`` macro from the `Managed Property Registrar Base <../_static/codedochtml/classot_1_1_managed_property_registrar_base.xhtml>`_ is used the generate the property.
A setter and getter method in the following syntax will be gerated:

.. code-block:: c++

    // Setter
    //
    // Syntax:
    // void set<PropertyObjectName>(<SetterArgumentType> _value);
    //
    // Example with:
    //     - Property Object Name: TreeEntityName
    //     - Property Type: String
    void setTreeEntityName(const std::String& _value);

    // Setter
    //
    // Syntax:
    // GetterArgumentType get<PropertyObjectName>(void) const;
    //
    // Example with:
    //     - Property Object Name: TreeEntityName
    //     - Property Type: String
    const std::string& getTreeEntityName(void) const;

A registrar to initialize the property and pass the optional read and write callback notifier will be generated aswell.
This registrar will create a `Managed Property Registrar Base <../_static/codedochtml/classot_1_1_managed_property_registrar_base.xhtml>`_
In the :ref:`code example <tree_entit_name_example_code>` a setter notifier is passed to notify the object whenever the property value has changed.

Notifier syntax:

.. code-block:: c++

    // Setter
    // is called after the write operation is completed.
    // void lambda(const ot::Property<Type>* _property);
    // e.g.
    void lambda(const ot::PropertyBool* _property);

    // Getter
    // is called before the read operation starts.
    // void lambda(const ot::Property<Type>* _property);
    // e.g.
    void lambda(const std::string& _propertyGroup, const std::string& _propertyName);

The property itself can be accessed at any time after object creation from the objects `Property Manager <../_static/codedochtml/classot_1_1_property_manager.xhtml>`_.
The property in the :ref:`code example <tree_entit_name_example_code>` can be accessed via the property name `Entity Name` from the general group in the 
`Property Manager <../_static/codedochtml/classot_1_1_property_manager.xhtml>`_.
This allows the property to be retreived and modified (e.g. change property flags, limits, ...).

.. _tree_entit_name_example_code:

Code
^^^^^

.. code-block:: c++
    
    // OpenTwin header
    #include "OTWidgets/TreeWidgetItem.h"
    #include "OTWidgets/ManagedWidgetPropertyObject.h"

    class TreeEntity : ot::TreeWidgetItem, public ot::ManagedWidgetPropertyObject {
    public:
	    OT_PROPERTY_STRING_CALL(
            TreeEntityName,                       // Object name
            "Entity Name",                        // Property name.
             ,                                    // No read callback
            [=](const ot::PropertyString* _str) { // Write callback
                // Argument type is PropertyString since a string
                // property was created. PropertyBool for bool and so on.
                // Notify this object about the name change.
                this->treeEntityNameHasChanged(_str->getValue()); 
            },
            "New Entity"                          // Initial value
        );

    protected:
        // Will be called every time the entity was written.
        // Inheriting classes could react to name chanes by overriding.
        virtual void treeEntityNameHasChanged(const std::string& _newName) {
            // Handle the name changes.
            // For example:

            // Ensure name is valid
            if (!this->isNameValid(_newName)) {
                // Handle invalid name
            }
            // Update tree item text if needed.
            const QString txt = QString::fromStdString(_newName);
            if (txt != this->text(0)) {
                this->setText(0, txt);
            }
        };

        // Example interface to validate an entity name.
        virtual bool isNameValid(const std::string& _name) const = 0;


    };

.. _property_linking:

Property Linking
****************

Libraries and applications that use the ``OTWidgets`` library can use the frontend property management
to enable different kinds of property handling.

Editing in Property Grid
========================

Property visualization in a property grid can be archieved by using the `Property Manager Grid Link <../_static/codedochtml/classot_1_1_managed_property_widget_object.xhtml>`_.
The grid link can be set to a `Widget Property Manager <../_static/codedochtml/classot_1_1_widget_property_manager.xhtml>`_ that is owned by a `Managed Widget Property Object <../_static/codedochtml/classot_1_1_managed_widget_property_object.xhtml>`_.
All properties that were added to the objects `Widget Property Manager <../_static/codedochtml/classot_1_1_widget_property_manager.xhtml>`_ will be visualized when calling the visualization method of the `Property Manager Grid Link <../_static/codedochtml/classot_1_1_managed_property_widget_object.xhtml>`_.
Changes to the property by the property grid will update the property stored in the `Widget Property Manager <../_static/codedochtml/classot_1_1_widget_property_manager.xhtml>`_.
Since the changes will be handled by the `Widget Property Manager <../_static/codedochtml/classot_1_1_widget_property_manager.xhtml>`_
the use of the `Property Manager Grid Link <../_static/codedochtml/classot_1_1_managed_property_widget_object.xhtml>`_
won't affect any set callback methods for property read and write operations.

.. code-block:: c++

    // OpenTwin header
    #include "OTWidgets/TreeWidgetItem.h"
    #include "OTWidgets/ManagedWidgetPropertyObject.h"

    class TreeEntity : ot::TreeWidgetItem, public ot::ManagedWidgetPropertyObject {
    public:
        // Example property from Managed Property Example 1.
	    OT_PROPERTY_STRING_CALL(TreeEntityName, "Entity Name", ,
            [=](const ot::PropertyString* _str)
            { this->treeEntityNameHasChanged(_str->getValue()); },
            "New Entity"
        );

    protected:
        // Will be called every time the entity was written
        virtual void treeEntityNameHasChanged(const std::string& _newName) {};

    private:
        // Grid link that is used.
        ot::PropertyManagerGridLink* m_gridLink;

    public:
        // In the constructor add the grid link
        TreeEntity() {
            m_gridLink = new ot::PropertyManagerGridLink(this->getPropertyManager());
        }

        // Whenever the entity should be displayed in the property grid call this method.
        void visualizeAtGrid(ot::PropertyGrid* _grid) {
            m_gridLink->visualizeAtGrid(_grid);
        }

        // When the property is not edited in the property grid call this method.
        // This should be called before another entity will visualize itself at the same grid.
        void forgetPropertyGrid(void) {
            m_gridLink->forgetPropertyGrid(void);
        }

        // Instead of the the methods "visualizeAtGrid" and "forgetPropertyGrid"
        // a getter to get the grid link could be added so the grid link handling
        // will take place elsewhere.
    };

