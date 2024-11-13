Table
=====

Setup
-----

A table can be created by sending the ``OT_ACTION_CMD_UI_TABLE_Setup`` action to the Frontend.
Note that calling Setup for an existing table will set the tables contents changed flag to false.

Modify
------

Set Saved
^^^^^^^^^

OT_ACTION_CMD_UI_TABLE_SetSaved

Set Modified
^^^^^^^^^^^^

OT_ACTION_CMD_UI_TABLE_SetModified

Insert Row After
^^^^^^^^^^^^^^^^

OT_ACTION_CMD_UI_TABLE_InsertRowAfter

Insert Row Before
^^^^^^^^^^^^^^^^^

OT_ACTION_CMD_UI_TABLE_InsertRowBefore

Remove Row
^^^^^^^^^^

OT_ACTION_CMD_UI_TABLE_RemoveRow

Insert Column After
^^^^^^^^^^^^^^^^^^^

OT_ACTION_CMD_UI_TABLE_InsertColumnAfter

Insert Column Before
^^^^^^^^^^^^^^^^^^^^

OT_ACTION_CMD_UI_TABLE_InsertColumnBefore

Remove Column
^^^^^^^^^^^^^

OT_ACTION_CMD_UI_TABLE_RemoveColumn

Close Table
^^^^^^^^^^^

OT_ACTION_CMD_UI_TABLE_Close

Set Current Selection
^^^^^^^^^^^^^^^^^^^^^

To modify the current selection of the table send the ``OT_ACTION_CMD_UI_TABLE_SetSelection`` action to the Frontend.
Note that the BasicServiceInformation must be provided via the action document.
Note that specified table must already exist.

.. list-table:: Action Parameters
   :header-rows: 1

   * - Action Parameter
     - Type
     - Description
   * - OT_ACTION_PARAM_NAME
     - std::string
     - Name of the table.
   * - OT_ACTION_PARAM_Ranges
     - JSON object list
     - Every object in the list must be a ot::TableRange.
       Every TableRange entry in the list contains a range that should be selected.
   * - OT_ACTION_PARAM_ClearSelection
     - Boolean
     - [Optional. Default = false] |br|
       If true the current selection will be removed and only the provided ranges will be selected. |br|
       If false the new ranges will be added to the current selection.

.. _table_get_current_selection:

Get Current Selection
^^^^^^^^^^^^^^^^^^^^^

To request a callback for the current table selection send the ``OT_ACTION_CMD_UI_TABLE_GetSelection`` action to the Frontend.
Since all actions to the Frontend are queued this action will be handled asynchronous.
Note that the BasicServiceInformation must be provided via the action document.
Note that specified table must already exist.

.. list-table:: Action Parameters
   :header-rows: 1

   * - Action Parameter
     - Type
     - Description
   * - OT_ACTION_PARAM_NAME
     - std::string
     - Name of the table.
   * - OT_ACTION_PARAM_SENDER_URL
     - std::string
     - Service URL where the callback should be send to.
   * - OT_ACTION_PARAM_MODEL_FunctionName
     - std::string
     - Name of the callback function that should be triggered in the service which receives the callback.

.. list-table:: Callback Parameters
   :header-rows: 1

   * - Action Parameter
     - Type
     - Description
   * - OT_ACTION_PARAM_NAME
     - std::string
     - Name of the table.
   * - OT_ACTION_PARAM_MODEL_FunctionName
     - std::string
     - Name of the callback function that should be triggered.
   * - OT_ACTION_PARAM_Ranges
     - JSON object list
     - Every object in the list is a ot::TableRange.
       Every TableRange entry in the list contains a range that is selected.

Set Current Selection Background
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

If the background color of the current table selection should be changed send the ``OT_ACTION_CMD_UI_TABLE_SetCurrentSelectionBackground`` action to the Frontend.
The current selection can be optionally changed when sending this action.
Note that the BasicServiceInformation must be provided via the action document.
Note that specified table must already exist.

.. list-table:: Action Parameters
   :header-rows: 1

   * - Action Parameter
     - Type
     - Description
   * - OT_ACTION_PARAM_NAME
     - std::string
     - Name of the table.
   * - OT_ACTION_PARAM_Color
     - ot::Color
     - The background color to apply.
   * - OT_ACTION_PARAM_ClearSelection 
     - Boolean
     - [Optional. Default = false] |br|
       If true the current selection will be cleared before applying the new color.
   * - OT_ACTION_PARAM_Ranges
     - JSON object list
     - [Optional. Default = empty] |br|
       If provided the specified ranges will be selected before applying the new color but after clearing the current selection if requested.
       Every object in the list is a ot::TableRange.
       Every TableRange entry in the list contains a range that is selected.
   * - OT_ACTION_PARAM_RequestCallback
     - Boolean
     - [Optional. Default = false] |br|
       If true the specified service will receive a callback after the operation is done.
       The callback is the same as in :ref:`Get Current Selection<table_get_current_selection>`
   * - OT_ACTION_PARAM_SENDER_URL
     - std::string
     - [Mandatory if RequestCallback = true] |br|
       Service URL where the callback should be send to.
   * - OT_ACTION_PARAM_MODEL_FunctionName
     - std::string
     - [Mandatory if RequestCallback = true] |br|
       Name of the callback function that should be triggered in the service which receives the callback.

