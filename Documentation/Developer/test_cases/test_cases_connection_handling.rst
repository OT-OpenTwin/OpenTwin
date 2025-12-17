Connection Handling
###################

Use a DataProcessing/CircuitSimulator/Hierarchical project type for the following tests.

.. note::
  Test the hierarchical service explicitly separately. Test the branch connection tests with the Circuit Simulator Service.

Connection tests
****************
  
.. list-table::
    :header-rows: 1

    * - #
      - Name
      - Setup
      - Execution
      - Expectation
      
    * - 1
      - Create connection
      - Open a project and select a Blockeditor.
      - * Drop any two blocks into the blockeditor.
        * Create a connection between the blocks.
      - Connection is created successfully without errors.

    * - 2
      - Delete connection
      - Open a project and select a Blockeditor with two connected blocks.
      - * Select the connection between the blocks.
        * Delete the connection.
      - Connection is deleted successfully without errors.

    
    * - 3
      - Create connection - Undo/Redo
      - Open a project and select a Blockeditor.
      - * Drop any two blocks into the blockeditor.
        * Create a connection between the blocks.
        * Undo the creation of the connection.
        * Redo the creation of the connection.
      - * Connection is created successfully after redo without errors.
        * Connection is removed successfully after undo without errors.

    * - 4
      - Delete connection - Undo/Redo
      - Open a project and select a Blockeditor with two connected blocks.
      - * Select the connection between the blocks.
        * Delete the connection.
        * Undo the deletion of the connection.
        * Redo the deletion of the connection.
      - * Connection is removed successfully after redo without errors.
        * Connection is restored successfully after undo without errors.

    * - 5
      - Change position of connection 
      - Open a project and select a Blockeditor with two connected blocks.
      - * Delete one connected block
        * Select the connector of the connection and change its position
      - * Connection position is changed successfully without errors.
        * A model state is created for changing the position of the connection.

    * - 6
      - Create a branch connection
      - Open a CircuitSimulator project and select a Circuit.
      - * Drop any three blocks into the blockeditor.
        * Create a branch connection from the first block to the other two blocks.
      - Branch connection is created successfully in one modelstate without errors.

    * - 7
      - Delete a branch connection
      - Open a CircuitSimulator project and select a Circuit with three blocks connected via a branch connection.
      - * Select any branch connection between the blocks.
        * Delete the branch connection.
      - Branch connection is deleted successfully without errors.

    * - 8
      - Create a branch connection - Undo/Redo
      - Open a CircuitSimulator project and select a Circuit.
      - * Drop any three blocks into the blockeditor.
        * Create a branch connection from the first block to the other two blocks.
        * Undo the creation of the branch connection.
        * Redo the creation of the branch connection.
      - * Branch connection is created successfully after redo without errors.
        * Branch connection is removed successfully after undo without errors.
        * Branch connection is created/removed in one model state.

    * - 9
      - Delete a branch connection - Undo/Redo
      - Open a CircuitSimulator project and select a Circuit with three blocks connected via a branch connection.
      - * Select any branch connection between the blocks.
        * Delete the branch connection.
        * Undo the deletion of the branch connection.
        * Redo the deletion of the branch connection.
      - * Branch connection is removed successfully after redo without errors.
        * Branch connection is restored successfully after undo without errors.
        * Branch connection is deleted/restored in one model state.
    
    * - 10
      - Change position of branch connection
      - Open a CircuitSimulator project and select a Circuit with three blocks connected via a branch connection.
      - * Select the connector of the branch connection and change its position
      - * Branch connection position is changed successfully without errors.
        * A model state is created for changing the position of the branch connection.

    * - 11
      - Create connection between Branch connector and Block
      - Open a CircuitSimulator project and select a Circuit with three blocks connected via a branch connection.
      - * Delete one connection of the branch connection.
        * Create a new connection between a block and the branch connector.
      - Connection is created successfully without errors.

Connection snapping tests
*************************
.. list-table::
    :header-rows: 1

    * - #
      - Name
      - Setup
      - Execution
      - Expectation
      
    * - 1
      - Connection unsnapping (one sided)
      - Open a project and select a Blockeditor with 2 unconnected blocks.
      - * Connect the blocks
        * Select the connection and unsnap the connection from one block.
      - Connection is unsnapped and is only connected to one block without errors.

    * - 2
      - Connection unsnapping (both sides)
      - Open a project and select a Blockeditor with 2 connected blocks.
      - * Select the connection and unsnap the connection from both blocks.
      - Connection is unsnapped from both blocks without errors.

    * - 3
      - Connection snapping (use dragging of the block)
      - Open a project and select a Blockeditor with 2 blocks and one connection which is connected with one block.
      - * Select the unconnected block and drag it to the connection to snap the connection to the block.
      - Connection is snapped without errors.

    * - 4
      - Connection snapping (use dragging of the connection)
      - Open a project and select a Blockeditor with 2 blocks and one connection which is connected with one block.
      - * Select the connection and drag it to the remained block to snap the connection to the unconnected block.
      - Connection is snapped without errors.
  
    * - 5
      - Branch connection unsnapping
      - Open a CircuitSimulator project and select a Circuit with 3 connected blocks via a branch connection.
      - * Select a branch connection and unsnap the connection from one block.
      - One connection of the branch is unsnapped from one block without errors while the other connections remain connected.

    * - 6
      - Branch connection snapping
      - Open a CircuitSimulator project and select a Circuit with 3 blocks which are connected via a branch connection
      - * Select a branch connection and unsnap one connection from one block.
        * Snap the unsnapped connection back to the block.
      - Connection is snapped back to the block without errors.

    * - 7
      - Position change of unsnapped connection
      - Open a project and select a Blockeditor with 2 blocks and one connection which is fully unsnapped.
      - * Select the connection and change its position.
      - * Connection position is changed successfully without errors.
        * A model state is created for changing the position of the connection.
    
    * - 8
      - Snap connection after block removed
      - Open a project and select a Blockeditor with 2 blocks and one connection which is connected to both blocks.
      - * Remove one block
        * Drop another block into the blockeditor
        * Snap the connection to the newly dropped block
      - Connection is snapped to the newly dropped block without errors.

    * - 9
      - Snap one connection to two blocks in single action (use dragging of the connection)
      - Open a project and select blockeditor with 2 blocks and one connection which is unconnected.
      - * Select the connection and snap it to both blocks in a single action.
      - Connection is snapped to both blocks without errors.

    * - 10
      - Snap one Connection to two blocks in single action (use dragging of the blocks)
      - Open a project and select blockeditor with 2 blocks and one connection which is unconnected.
      - * Select both blocks and drag them to the connection to snap the connection to both blocks in a single action.
      - Connection is snapped to both blocks without errors.

    * - 11
      - Snap two or more connections to one block in single action
      - Open a CircuitSimulator project and select Circuit with 3 blocks and two connections which are unconnected.
      - * Select both connections and drag them to the block to snap both connections to the block in a single action.
      - Both connections are snapped to the block without errors.

    * - 12
      - Snap two or more connections to one block in single action (use dragging of the block)
      - Open a CircuitSimulator project and select Circuit with 3 blocks and two connections which are unconnected.
      - * Select the block and drag it to both connections to snap both connections to the block in a single action.
      - Both connections are snapped to the block without errors.





    
 
