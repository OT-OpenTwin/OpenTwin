//! @file ModelLibraryDialogCfg.cpp
//! @author Sebastian Urmann	
//! @date June 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/ModelLibraryDialogCfg.h"

ot::ModelLibraryDialogCfg::ModelLibraryDialogCfg(const std::list<LibraryModel>& _models, const std::list<std::string> _filter) :
	m_models(_models), m_filter(_filter) 
{}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter

void ot::ModelLibraryDialogCfg::addModel(const LibraryModel& _model) {
	m_models.push_back(_model);
}

void ot::ModelLibraryDialogCfg::addModel(LibraryModel&& _model) {
	m_models.push_back(std::move(_model));
}

void ot::ModelLibraryDialogCfg::addFilter(const std::string& _filter) {
	if (std::find(m_filter.begin(), m_filter.end(), _filter) == m_filter.end()) {
		m_filter.push_back(_filter);
	}
}

void ot::ModelLibraryDialogCfg::addFilter(std::string&& _filter) {
	if (std::find(m_filter.begin(), m_filter.end(), _filter) == m_filter.end()) {
		m_filter.push_back(std::move(_filter));
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Serialization

void ot::ModelLibraryDialogCfg::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	JsonArray modelArr;
	for (const LibraryModel& model : m_models) {
		JsonObject childObj;
		model.addToJsonObject(childObj, _allocator);
		modelArr.PushBack(childObj, _allocator);
	}
	_object.AddMember("Models", modelArr, _allocator);
	_object.AddMember("Filter", JsonArray(m_filter, _allocator), _allocator);
}

void ot::ModelLibraryDialogCfg::setFromJsonObject(const ot::ConstJsonObject& _object) {

	m_models.clear();
	for (const ConstJsonObject& obj : json::getObjectList(_object, "Models")) {
		LibraryModel newModel;
		newModel.setFromJsonObject(obj);
		m_models.push_back(newModel);
	}

	m_filter = json::getStringList(_object, "Filters");
}
