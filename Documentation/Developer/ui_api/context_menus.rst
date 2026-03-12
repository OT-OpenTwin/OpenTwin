Context Menu Proposal
#####################

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

.. image:: images/context_menu_full_cycle_seq.drawio.svg
   :alt: Context menu roundtrip
