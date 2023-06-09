#include "OTGui/LinearGradientPainter2D.h"
#include "OpenTwinCore/rJSONHelper.h"

ot::LinearGradientPainter2DStep::LinearGradientPainter2DStep()
	: m_x(0.), m_y(0.) {}

ot::LinearGradientPainter2DStep::LinearGradientPainter2DStep(double _x, double _y, const ot::Color& _color) 
	: m_x(_x), m_y(_y), m_color(_color) {}

ot::LinearGradientPainter2DStep::LinearGradientPainter2DStep(const LinearGradientPainter2DStep& _other) 
	: m_x(_other.m_x), m_y(_other.m_y), m_color(_other.m_color) {}

ot::LinearGradientPainter2DStep::~LinearGradientPainter2DStep() {}

ot::LinearGradientPainter2DStep& ot::LinearGradientPainter2DStep::operator = (const LinearGradientPainter2DStep& _other) {
	m_x = _other.m_x;
	m_y = _other.m_y;
	m_color = _other.m_color;

	return *this;
}

bool ot::LinearGradientPainter2DStep::operator == (const LinearGradientPainter2DStep& _other) const {
	return m_x == _other.m_x && m_y == _other.m_y && m_color == _other.m_color;
}

bool ot::LinearGradientPainter2DStep::operator != (const LinearGradientPainter2DStep& _other) const {
	return m_x == _other.m_x || m_y != _other.m_y || m_color != _other.m_color;
}

void ot::LinearGradientPainter2DStep::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	ot::rJSON::add(_document, _object, "X", m_x);
	ot::rJSON::add(_document, _object, "Y", m_y);
	OT_rJSON_createValueObject(colorObj);
	m_color.addToJsonObject(_document, colorObj);
	ot::rJSON::add(_document, _object, "Color", colorObj);
}

void ot::LinearGradientPainter2DStep::setFromJsonObject(OT_rJSON_val& _object) {
	OT_rJSON_checkMember(_object, "X", Double);
	OT_rJSON_checkMember(_object, "Y", Double);
	OT_rJSON_checkMember(_object, "Color", Object);
	m_x = _object["X"].GetDouble();
	m_y = _object["Y"].GetDouble();
	OT_rJSON_val colorObj = _object["Color"].GetObject();
	m_color.setFromJsonObject(colorObj);
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::LinearGradientPainter2D::LinearGradientPainter2D() {}

ot::LinearGradientPainter2D::LinearGradientPainter2D(const std::vector<LinearGradientPainter2DStep>& _steps) : m_steps(_steps) {}

ot::LinearGradientPainter2D::LinearGradientPainter2D(const LinearGradientPainter2D& _other) : m_steps(_other.m_steps) {}

ot::LinearGradientPainter2D::~LinearGradientPainter2D() {}

ot::LinearGradientPainter2D& ot::LinearGradientPainter2D::operator = (const LinearGradientPainter2D& _other) {
	m_steps = _other.m_steps;
	return *this;
}

bool ot::LinearGradientPainter2D::operator == (const LinearGradientPainter2D& _other) const {
	if (m_steps.size() != _other.m_steps.size()) return false;
	for (size_t i = 0; i < m_steps.size(); i++) {
		if (m_steps[i] != _other.m_steps[i]) return false;
	}
	return true;
}

bool ot::LinearGradientPainter2D::operator != (const LinearGradientPainter2D& _other) const {
	return !(*this == _other);
}

void ot::LinearGradientPainter2D::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	ot::Painter2D::addToJsonObject(_document, _object);
	OT_rJSON_createValueArray(stepArr);
	for (auto s : m_steps) {
		OT_rJSON_createValueObject(stepObj);
		s.addToJsonObject(_document, stepObj);
		stepArr.PushBack(stepObj, _document.GetAllocator());
	}
	ot::rJSON::add(_document, _object, "Steps", stepArr);
}

void ot::LinearGradientPainter2D::setFromJsonObject(OT_rJSON_val& _object) {
	ot::Painter2D::setFromJsonObject(_object);
	OT_rJSON_checkMember(_object, "Steps", Array);
	OT_rJSON_val stepArr = _object["Steps"].GetArray();
	m_steps.clear();
	for (rapidjson::SizeType i = 0; i < stepArr.Size(); i++) {
		OT_rJSON_checkArrayEntryType(stepArr, i, Object);
		OT_rJSON_val stepObj = stepArr[i].GetObject();
		LinearGradientPainter2DStep newStep;
		newStep.setFromJsonObject(stepObj);
		m_steps.push_back(newStep);
	}
}

void ot::LinearGradientPainter2D::addStep(const LinearGradientPainter2DStep& _step) {
	m_steps.push_back(_step);
}

