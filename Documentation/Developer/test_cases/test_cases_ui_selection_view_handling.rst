Frontend Selection and View Handling
####################################

.. list-table::
    :header-rows: 1

    * - #
      - Name
      - Setup
      - Execution
      - Expectation
    
    * - 1
      - New View from Selection
      - No selection
      - Select a visualizable entity (e.g. PythonScript)
      - - View opens ✔
        - Entity remains selected ✔

    * - 2
      - Remove Selection from View Close
      - - Visualizable Entity selected (e.g. PythonScript)
        - Corresponding view is open
      - Close the view
      - Entity deselected ✔

    * - 3 
      - Selection restore
      - - Visualizable Entity selected (e.g. PythonScript)
        - Corresponding view pinned
      - Change to another central view and then back
      - Selection of the view is restored without opening new views ✔

    * - 4
      - Selection information updated
      - Visualizable Entity selected and at least one other
      - Change selection so that the entity view remains open
      - Selection information is stored correctly (check via test #3) ✔

    * - 5
      - Range Selection triggers new view
      - - No selection
        - 3D view is focused
      - Select Series Metadata with Ctrl pressed
      - - Table view opens ✔
        - Range selection is not saved for 3D view (check by changing back to 3D view) ❌
    
    * - 6
      - Range Selection triggers new view 2
      - Range entity selected (e.g. SeriesMetadata)
      - Select other Range of other Table with Ctrl pressed
      - - Other table view opens with range selected
        - Old range selection is cleared and only new selection is stored at new view ✔
        - New selection is not stored at old view ❌



Rework notes:
*************

Navigation tree selection -> Forward to viewer

View focused -> restore selection from info

Viewer selection set notification -> SceneNode notification -> Visualizer notification -> add selected node to view selection info (api call)
Viewer selection unset notification -> SceneNode notification -> Visualizer notification -> remove selected node from view selection info (api call)

