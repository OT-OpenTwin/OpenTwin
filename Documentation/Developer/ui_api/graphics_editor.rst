Graphics Editor
===============

Create Items
^^^^^^^^^^^^

.. code-block:: c++

   #include "OTGui/GraphicsItemCfg.h"

   void foo(void) {
      
      

   }

Create Flow Blocks
^^^^^^^^^^^^^^^^^^

.. code-block:: c++

   #include "OTGui/GraphicsPackage.h"

   void foo(void) {
      
      

   }

Create Categories
^^^^^^^^^^^^^^^^^

.. code-block:: c++

   #include "OTGui/GraphicsCollectionCfg.h"

   void foo(void) {
      // Create root collection
      ot::GraphicsCollectionCfg* root = new ot::GraphicsCollectionCfg("Root", "Root category");

      // Create A
      ot::GraphicsCollectionCfg* categoryA = new ot::GraphicsCollectionCfg("A", "Child category A");

      ot::GraphicsCollectionCfg* categoryA1 = new ot::GraphicsCollectionCfg("A.1", "Child category A.1");
      ot::GraphicsCollectionCfg* categoryA2 = new ot::GraphicsCollectionCfg("A.2", "Child category A.2");

      categoryA->addChildCollection(categoryA1);
      categoryA->addChildCollection(categoryA2);
      root->addChildCollection(categoryA);

      // Create B
      ot::GraphicsCollectionCfg* categoryB = new ot::GraphicsCollectionCfg("B", "Child category B");
      ot::GraphicsCollectionCfg* categoryB1 = new ot::GraphicsCollectionCfg("B.1", "Child category B.1");
      ot::GraphicsCollectionCfg* categoryB2 = new ot::GraphicsCollectionCfg("B.2", "Child category B.2");

      categoryB->addChildCollection(categoryB1);
      categoryB->addChildCollection(categoryB2);
      root->addChildCollection(categoryB);

      // Create items
      ...

      // Add items
      categoryA1->addItem(newItem); // newItem must be a ot::GraphicsItemCfg object
      ...
   }

Fill item picker
^^^^^^^^^^^^^^^^

First the item picker must be filled.
The item picker displays the different items for the user where he can drag them into a graphics view.

.. code-block:: c++

   #include "OTCore/Owner.h"
   #include "OTCore/rJSON.h"
   #include "OTGui/GraphicsPackage.h"

   void foo(void) {
      // Create collections and their items
      ...

      // Add created collections to package
      GraphicsCollectionPackage pckg;
      pckg.addCollection(rootCollection1);
      pckg.addCollection(rootCollection2);

      // Create the request that will be send to the UI:

         // (1) Create request document
         OT_rJSON_createDOC(requestObj);  
         ot::rJSON::add(requestObj, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_GRAPHICSEDITOR_FillItemPicker);

         // (2) Add package
         OT_rJSON_createValueObject(pckgObj);
         pckg.addToJsonObject(requestObj, pckgObj);
         ot::rJSON::add(requestObj, OT_ACTION_PARAM_GRAPHICSEDITOR_Package, pckgObj);

         // (3) Add required service information
         ot::GlobalOwner::instance().addToJsonObject(requestObj, requestObj);

         // (4) Create request
         std::string request = ot::rJSON::toJSON(requestObj);
   }


Add Items to View
^^^^^^^^^^^^^^^^^

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
^^^^^^^^^^^^^^^^^^^^^^

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
^^^^^^^^^^^^^^^

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
^^^^^^^^^^^^^^^^^^

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

=============
Configuration
=============

A configuration consists of an Array containing ``Block Category`` objects.
Every provided ``Block Category`` object will be added to the Block Picker Navigation as a root item.

.. note::
   If providing a ``Block Category`` with an already existing name, the affected categories will be merged.

.. warning::
   If providing items with the same name, any duplicates will be ignored.
