#include "OTBlockEntities/EntityBlockLibraryInterface.h"


ot::EntityBlockLibraryInterface::EntityBlockLibraryInterface() {
}

std::list<ot::LibraryElement> ot::EntityBlockLibraryInterface::libraryElementWasSet(const ot::LibraryElement& _libraryElement, EntityBase* _entity, ot::NewModelStateInfo& _newStateInfo) {
	return {};
}
