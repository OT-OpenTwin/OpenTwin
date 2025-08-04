Model service
=============

Actions
-------

.. list-table:: Actions Overview
    :widths: 25 25 50
    :header-rows: 1

    * - Endpoint
      - Brief Description
      - Macro
    * - :ref:`Model.Delete <reference-model-delete>`
      - Deletes a model.
      - OT_ACTION_CMD_MODEL_Delete
    * - :ref:`Project.Save <reference-project-save>`
      - Saves a project in the database.
      - OT_ACTION_CMD_PROJ_Save
    * - :ref:`Model.Event.SelectionChanged <reference-model-event-selection-changed>`
      - Changes a selection.
      - OT_ACTION_CMD_MODEL_SelectionChanged
    * - :ref:`Model.Event.ItemRenamed <reference-model-event-item-renamed>`
      - Renames a model item.
      - OT_ACTION_CMD_MODEL_ItemRenamed
    * - :ref:`VisualizationModel.Set <reference-visualization-model-set>`
      - Visualizes a model.
      - OT_ACTION_CMD_SetVisualizationModel
    * - :ref:`VisualizationModel.Get <reference-visualization-model-get>`
      - Gets the visualization Model ID.
      - OT_ACTION_CMD_GetVisualizationModel
    * - :ref:`Model.State.Modified.Get <reference-model-state-modified-get>`
      - Checks if the model has been modified.
      - OT_ACTION_CMD_MODEL_GetIsModified
    * - :ref:`UI.Table.Setup <reference-ui-table-setup>`
      - Shows a table view in the UI.
      - OT_ACTION_CMD_UI_TABLE_Setup
    * - :ref:`Model.Visualisation.Data <reference-model-visualisation-data>`
      - Shows a data view in the UI.
      - OT_ACTION_CMD_MODEL_RequestVisualisationData

----

.. _reference-model-delete:

Model.Delete
^^^^^^^^^^^^

Brief description
"""""""""""""""""

Deletes a model.

Message Type
""""""""""""

mTLS

Parameters
""""""""""

No parameters are required.

Response
""""""""

An empty String.

----

.. _reference-project-save:

Project.Save
^^^^^^^^^^^^

Brief description
"""""""""""""""""

Saves a project in the database.

Message Type
""""""""""""

mTLS

Parameters
""""""""""

No parameters are required.

Response
""""""""

In case of success will return an empty String.

In case of error will return a String with prefix "Error: ".

----

.. _reference-model-event-selection-changed:

Model.Event.SelectionChanged
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Brief description
"""""""""""""""""

Changes a selection.

Message Type
""""""""""""

mTLS

Parameters
""""""""""

No parameters are required.

Response
""""""""

An empty String.

----

.. _reference-model-event-item-renamed:

Model.Event.ItemRenamed
^^^^^^^^^^^^^^^^^^^^^^^

Brief description
"""""""""""""""""

Renames a model item.

Message Type
""""""""""""

mTLS

Parameters
""""""""""

.. list-table::
    :widths: 25 25 50 50
    :header-rows: 1

    * - Name
      - Type
      - Description
      - Macro
    * - Model.ID
      - Unsigned Integer 64
      - Model ID (if model exists)
      - OT_ACTION_PARAM_MODEL_ID
    * - Model.Item.Name
      - String
      - New name (if model exists)
      - OT_ACTION_PARAM_MODEL_ITM_Name

Response
""""""""

In case of success will return an empty String.

In case of error will return a String with prefix "Error: ".

----

.. _reference-visualization-model-set:

VisualizationModel.Set
^^^^^^^^^^^^^^^^^^^^^^

Brief description
"""""""""""""""""

Opens a project and visualizes a model.

Message Type
""""""""""""

mTLS

Parameters
""""""""""

.. list-table::
    :widths: 25 25 50 50
    :header-rows: 1

    * - Name
      - Type
      - Description
      - Macro
    * - Model.ID
      - Unsigned Integer 64
      - Model ID (if model exists)
      - OT_ACTION_PARAM_MODEL_ID
    * - View.ID
      - Unsigned Integer 64
      - View ID
      - OT_ACTION_PARAM_VIEW_ID

Response
""""""""

In case of success will return an empty String.

In case of error will return a String with prefix "Error: ".

----

.. _reference-visualization-model-get:

VisualizationModel.Get
^^^^^^^^^^^^^^^^^^^^^^

Brief description
"""""""""""""""""

Gets the visualization Model ID.

Message Type
""""""""""""

mTLS

Parameters
""""""""""
No parameters are required.

Response
""""""""
.. list-table::
    :widths: 25 25 50 50
    :header-rows: 1

    * - Name
      - Type
      - Description
      - Macro
    * - BaseTypes.UID
      - Unsigned Integer 64
      - Visualization Model ID (if successful)
      - OT_ACTION_PARAM_BASETYPE_UID

In case of error will return a String with prefix "Error: ".

-----

.. _reference-model-state-modified-get:

Model.State.Modified.Get
^^^^^^^^^^^^^^^^^^^^^^^^

Brief description
"""""""""""""""""

Checks if the model has been modified.

Message Type
""""""""""""

mTLS

Parameters
""""""""""
No parameters are required.

Response
""""""""
.. list-table::
    :widths: 25 25 50 50
    :header-rows: 1

    * - Name
      - Type
      - Description
      - Macro
    * - BaseTypes.Boolean
      - Boolean
      - Is model modified (if successful)
      - OT_ACTION_PARAM_BASETYPE_Bool

In case of error will return a String with prefix "Error: ".

----

.. _reference-ui-table-setup:

UI.Table.Setup
^^^^^^^^^^^^^^

Brief description
"""""""""""""""""

Shows a data view in the UI.

Message Type
""""""""""""

mTLS

Parameters
""""""""""

.. list-table::
    :widths: 25 25 50 50
    :header-rows: 1

    * - Name
      - Type
      - Description
      - Macro
    * - Name
      - String
      - Table name
      - OT_ACTION_PARAM_NAME
    * - View.ActiveView.Set
      - Boolean
      - Is view set active
      - OT_ACTION_PARAM_VIEW_SetActiveView

Response
""""""""

A String containing an :ref:`ot::ReturnMessage <reference-ot-return-message>`.

-----

.. _reference-model-visualisation-data:

Model.Visualisation.Data
^^^^^^^^^^^^^^^^^^^^^^^^

Brief description
"""""""""""""""""

Shows a data view in the UI.

Message Type
""""""""""""

mTLS

Parameters
""""""""""

.. list-table::
    :widths: 25 25 50 50
    :header-rows: 1

    * - Name
      - Type
      - Description
      - Macro
    * - Model.Entity.ID
      - Unsigned Integer 64
      - Entity ID
      - OT_ACTION_PARAM_MODEL_EntityID
    * - Model.Event.Functions.Name
      - String
      - Visualisation type
      - OT_ACTION_PARAM_MODEL_FunctionName
    * - View.ActiveView.Set
      - Boolean
      - Is view set active
      - OT_ACTION_PARAM_VIEW_SetActiveView

Response
""""""""

A String containing an :ref:`ot::ReturnMessage <reference-ot-return-message>`.
