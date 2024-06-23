Graphics Editor
===============

Create Items
------------

Graphics items are "created" via configurations.
Every type of item has its own configuration implementation in the ``OTGui`` library.
These configurations are then used in the Frontend by the ``OTWidgets`` library to create the actual item.
For more information read the ``ot::GraphicsItemCfg`` code documentation.

Fill item picker
----------------

The item picker is a tree structure containing the item information sorted in collections.
To fill the item picker a ``ot::GraphicsPickerCollectionPackage`` must be provided.
The collection package contains a list of (potentially nested) ``ot::GraphicsPickerCollectionCfg`` where every collection may be seen as a "folder".
The collection contains child collections and a list of ``ot::GraphicsPickerCollectionCfg::ItemInformation``.
Every information entry represents an item that the user can pick.
A item information consists of the item name (that is used in the :ref:`Add Item Callback<add_item_callback>`), the item title that is displayed to the user and the preview icon.

Add Items to View
-----------------

.. code-block:: c++

   #include "OTCore/Owner.h"
   #include "OTGui/GraphicsPackage.h"

   void foo(void) {
      // Create items to add to the scene
      ...

      // Create package
      ot::GraphicsScenePackage pckg("My Graphics View Name");
      pckg.addItem(myFirstItem);
      pckg.addItem(mySecondItem);
      ...

      // Create request
      OT_rJSON_createDOC(reqDoc);
      OT_rJSON_createValueObject(pckgObj);
      pckg.addToJsonObject(reqDoc, pckgObj);

      ot::rJSON::add(reqDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_GRAPHICSEDITOR_AddItem);
      ot::rJSON::add(reqDoc, OT_ACTION_PARAM_GRAPHICSEDITOR_Package, pckgObj);
      ot::GlobalOwner::instance().addToJsonObject(reqDoc, reqDoc);

      // Create request that should be send to the UI
      std::string request = ot::rJSON::toJSON(reqDoc);

      // Send request
      ...
   }

Remove Items from View
----------------------

.. code-block:: c++

   #include "OTCore/Owner.h"

   void foo(void) {
      std::list<std::string> items;

      // Add item UIDs to the list above (items to be removed)
      ...

      // Create request
      OT_rJSON_createDOC(reqDoc);
      ot::rJSON::add(reqDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_GRAPHICSEDITOR_RemoveItem);
      ot::rJSON::add(reqDoc, OT_ACTION_PARAM_GRAPHICSEDITOR_EditorName, "My Graphics View Name");
      ot::rJSON::add(reqDoc, OT_ACTION_PARAM_GRAPHICSEDITOR_ItemIds, items);
      ot::GlobalOwner::instance().addToJsonObject(reqDoc, reqDoc);

      // Create request that should be send to the UI
      std::string request = ot::rJSON::toJSON(reqDoc);

      // Send request
      ...
   }

Add Connections
---------------

.. code-block:: c++

   #include "OTCore/Owner.h"
   #include "OTGui/GraphicsPackage.h"

   void foo(void) {
      // Create package
      ot::GraphicsConnectionPackage pckg("My Graphics View Name");

      // Fill package
      pckg.addConnection(sourceUid, sourceName, destinationUid, destinationName);
      ...

      // Create request
      OT_rJSON_createDOC(reqDoc);
      OT_rJSON_createValueObject(pckgObj);
      pckg.addToJsonObject(reqDoc, pckgObj);

      ot::rJSON::add(reqDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_GRAPHICSEDITOR_AddConnection);
      ot::rJSON::add(reqDoc, OT_ACTION_PARAM_GRAPHICSEDITOR_Package, pckgObj);
      ot::GlobalOwner::instance().addToJsonObject(reqDoc, reqDoc);

      // Create request that should be send to the UI
      std::string request = ot::rJSON::toJSON(reqDoc);

      // Send request
      ...
   }

Remove Connections
------------------

.. code-block:: c++

   #include "OTCore/Owner.h"
   #include "OTGui/GraphicsPackage.h"

   void foo(void) {
      // Create package
      ot::GraphicsConnectionPackage pckg("My Graphics View Name");

      // Fill package
      pckg.addConnection(sourceUid, sourceName, destinationUid, destinationName);
      ...

      // Create request
      OT_rJSON_createDOC(reqDoc);
      OT_rJSON_createValueObject(pckgObj);
      pckg.addToJsonObject(reqDoc, pckgObj);

      ot::rJSON::add(reqDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_GRAPHICSEDITOR_RemoveConnection);
      ot::rJSON::add(reqDoc, OT_ACTION_PARAM_GRAPHICSEDITOR_Package, pckgObj);
      ot::GlobalOwner::instance().addToJsonObject(reqDoc, reqDoc);

      // Create request that should be send to the UI
      std::string request = ot::rJSON::toJSON(reqDoc);

      // Send request
      ...
   }

Callbacks
---------

The Graphics API provides the following actions as callbacks:

- :ref:`OT_ACTION_CMD_UI_GRAPHICSEDITOR_AddItem<add_item_callback>`
- :ref:`OT_ACTION_CMD_UI_GRAPHICSEDITOR_AddConnection<add_connection_callback>`
- :ref:`OT_ACTION_CMD_UI_GRAPHICSEDITOR_ItemChanged<item_changed_callback>`

The actions are defined in ``OTCommunication/ActionTypes.h``.

.. note::
   
   Note that remove requests are handled via the ModelEntities and not via the Graphics API.


.. _add_item_callback:

Add Item Callback
^^^^^^^^^^^^^^^^^

.. list-table:: Action Parameters
   :header-rows: 1

   * - Action Parameter
     - Type
     - Description
   * - OT_ACTION_PARAM_GRAPHICSEDITOR_ItemName
     - String
     - The name of the requested item.
   * - OT_ACTION_PARAM_GRAPHICSEDITOR_ItemPosition
     - ot::Point2DD
     - The position the user dropped the item at.
   * - OT_ACTION_PARAM_GRAPHICSEDITOR_EditorName
     - String
     - Name of the Graphics Editor.

The add item action is sent whenever the user dropped a Graphics Item on the View.

.. _add_connection_callback:

Add Connection Callback
^^^^^^^^^^^^^^^^^^^^^^^

.. list-table:: Action Parameters
   :header-rows: 1

   * - Action Parameter
     - Type
     - Description
   * - OT_ACTION_PARAM_GRAPHICSEDITOR_Package
     - ot::GraphicsConnectionPackage
     - Package containing all connections and the editor name.

The add connection action is sent when the user has created a valid connection in the Graphics Editor.
A valid connection is a connection that has a valid origin and destination item and the origin and destination are not equal.

.. _item_changed_callback:

Item Changed Callback
^^^^^^^^^^^^^^^^^^^^^

.. list-table:: Action Parameters
   :header-rows: 1

   * - Action Parameter
     - Type
     - Description
   * - OT_ACTION_PARAM_Config
     - ot::GraphicsItemCfg
     - The current item configuration.
   * - OT_ACTION_PARAM_GRAPHICSEDITOR_EditorName
     - String
     - Name of the Graphics Editor.

The change action is sent whenever the current item configuration has changed.
For example the item has been moved (after the move operation is finished), transformed, renamed, and so on.
In general, whenever any member of the ``ot::GraphicsItemCfg`` has changed in the Frontend the item changed action is sent.
