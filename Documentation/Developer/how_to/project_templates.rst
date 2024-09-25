Project Templates
=================

Collection Syntax
-----------------

A template collection requires a document with an empty `Information` field.
This document may contain the following entries:

.. list-table::
    :header-rows: 1

    * - Entry
      - Type
      - Description
    
    * - ProjectType
      - String
      - The project type accosiated with this template. This type will be used when creating a new session based on this template.
    
    * - BriefDescription
      - String
      - A short description of the template. This description will be displayed in a ToolTip when hovering over this template in the `Create New Project` dialog.
    
    * - Description
      - String
      - A detailed description of the template. This description will be displayed in the `Create New Project` dialogs' info box.

