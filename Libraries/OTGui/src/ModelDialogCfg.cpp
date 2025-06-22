#include "OTGui/ModelDialogCfg.h"

#define JSON_MEMBER_Models "Models"
#define JSON_MEMBER_Filter "Filter"

ot::ModelDialogCfg::ModelDialogCfg(const std::list<LibraryModel>& _models, const std::list<std::string> _filter) : m_models(_models), m_filter(_filter) {
}

void ot::ModelDialogCfg::addModel(const LibraryModel& _model) {
	m_models.push_back(_model);
}

void ot::ModelDialogCfg::setModelList(const std::list<LibraryModel>& _models) {
	m_models = _models;
}

void ot::ModelDialogCfg::addFilter(const std::string& _filter) {
	m_filter.push_back(_filter);
}

void ot::ModelDialogCfg::setFilterList(const std::list<std::string>& _filters) {
	m_filter = _filters;
}

void ot::ModelDialogCfg::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	JsonArray modelArr;
	for (const LibraryModel& m : m_models) {
		JsonObject childObj;
		m.addToJsonObject(childObj, _allocator);
		modelArr.PushBack(childObj, _allocator);
	}
	_object.AddMember(JSON_MEMBER_Models, modelArr, _allocator);

	JsonArray filterArr;
	for (const std::string& f : m_filter) {
		filterArr.PushBack(JsonString(f, _allocator), _allocator);
	}
	_object.AddMember(JSON_MEMBER_Filter, filterArr, _allocator);
}

void ot::ModelDialogCfg::setFromJsonObject(const ot::ConstJsonObject& _object) {

	m_models.clear();
	ConstJsonArray modelArr = json::getArray(_object, JSON_MEMBER_Models);
	for (rapidjson::SizeType c = 0; c < modelArr.Size(); c++) {
		ConstJsonObject childObj = json::getObject(modelArr, c);
		LibraryModel newModel;
		newModel.setFromJsonObject(childObj);
		m_models.push_back(newModel);
	}

	m_filter.clear();
	ConstJsonArray filterArr = json::getArray(_object, JSON_MEMBER_Filter);
	for (rapidjson::SizeType c = 0; c < filterArr.Size(); c++) {
		m_filter.push_back(json::getString(filterArr, c));
	}
	

}
