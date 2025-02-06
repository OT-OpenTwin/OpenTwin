Model Entities
==============

Results of operations are serialised in so called Entities. We separate the Entities in different categories. Currently we have BlockEntities, ModelEntities and CADEntities. 
Each category has its own library since they have dependencies on rather large thrid-party library.


Data and topology Entities
--------------------------

Base Classes
------------
| All entities must inherit from EntityBase. A special case arises when a topology entity has entries listed beneath it in the navigation tree—these entries are considered its child entities.
| When the parent entity is deleted, all its children are automatically removed as well. Entities that can have children must inherit from EntityContainer to support this hierarchical structure.
| Managing parent-child relationships does not need to be handled during entity creation. Instead, the ModelService is responsible for this process, as it oversees entity deletion and ensures proper relationship management.


Class Foctory
-------------