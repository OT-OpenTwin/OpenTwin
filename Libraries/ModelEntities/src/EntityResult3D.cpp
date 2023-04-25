#include "../include/EntityResult3D.h"

EntityResult3D::EntityResult3D(ot::UID ID, EntityBase * parent, EntityObserver * obs, ModelState * ms, ClassFactory * factory, const std::string & owner)
	: EntityVis2D3D(ID, parent, obs, ms, factory, owner) {}
