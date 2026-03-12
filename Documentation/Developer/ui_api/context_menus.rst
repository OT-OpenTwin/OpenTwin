Context Menu API
################

Where are context menus needed?
*******************************

The following lists show widgets that could have a context menu.  

Preferred widgets
=================

.. list-table::
   :header-rows: 1

   * - Name
     - Context
     - Options
   * - Output
     - 
     - - Select All
       - Clear
   * - Navigation Tree
     - Hovered item
     - - Delete
       - Clone **\***
       - Rename **\***
       - Visualize **\*\***
   * - 3D View
     - - Geometry
       - Face
       - Edge
     - - Delete
       - Hide
       - ...
   * - Graphics View
     - - Item
       - Connection
     - 

**\*** would be a good feature to add  

**\*\*** A new *Visualization Mode* flag (or similar) would allow disabling visualization
for calculation / data intensive requests on entity selection.

Unclear
=======

.. list-table::
   :header-rows: 1

   * - Name
     - Context
     - Options
   * - Property Grid?
     - - Property
     - 
   * - Tool Bar Buttons?
     - 
     - 
   * - Text Editor?
     - 
     - Example: |br|
       Python script: *Add breakpoint here*
   * - Table?
     - 
     - 
   * - Plot
     - 
     - Question: |br|
       Will the property grid and modal tool bar be enough?
   * - Version Graph
     - 
     - Examples: |br|
       - Rename version
       - Create branch

Note:

It might be a useful feature to explicitly create a new branch in order to avoid
misclicking in the *"delete all other data in branch irreversibly :D"* dialog.

Context Menu Roundtrip
**********************

The figure below shows all relevant classes involved in displaying a context menu for a widget and reacting to its button press.

.. image:: images/context_menu_full_cycle_seq.drawio.svg
   :alt: Context menu roundtrip

There are five main steps:

#. Widget raises event and the wrapper class creates the corresponding **1:** ``ContextRequestWidgetEvent (CRWE)`` instance.
#. ``CRWE`` creates ``ContextRequestData`` and sets it to the **2:** ``ContextMenuRequestEvent (CMRE)``.
#. Event is dispatched to the callback service (e.g. Model) which then returns a **3:** ``MenuCfg`` configuration.
#. Configuration (3) is used to create and display the **4:** ``ContextMenu``.
#. User selected action is used to perform default action or send owner notification via the ``ContextMenuCallbackBase (CMCB)``.

