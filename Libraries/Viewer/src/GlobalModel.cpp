#include "stdafx.h"
#include "GlobalModel.h"

Model* GlobalModel::instance() {
	return GlobalModel::getInstance();
}

void GlobalModel::setInstance(Model* _model) {
	GlobalModel::getInstance() = _model;
}

Model*& GlobalModel::getInstance(void) {
	static Model* g_instance{ nullptr };
	return g_instance;
}
