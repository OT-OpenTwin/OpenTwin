//! @file FillPainter2D.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/rJSONHelper.h"
#include "OTGui/FillPainter2D.h"

static ot::SimpleFactoryRegistrar<ot::FillPainter2D> fillCfg(OT_SimpleFactoryJsonKeyValue_FillPainter2DCfg);

ot::FillPainter2D::FillPainter2D() {}

ot::FillPainter2D::FillPainter2D(const ot::Color& _color) : m_color(_color) {}

ot::FillPainter2D::~FillPainter2D() {}

void ot::FillPainter2D::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	ot::Painter2D::addToJsonObject(_document, _object);
	OT_rJSON_createValueObject(colObj);
	m_color.addToJsonObject(_document, colObj);
	ot::rJSON::add(_document, _object, "Color", colObj);
}

void ot::FillPainter2D::setFromJsonObject(OT_rJSON_val& _object) {
	ot::Painter2D::setFromJsonObject(_object);
	OT_rJSON_checkMember(_object, "Color", Object);
	OT_rJSON_val colObj = _object["Color"].GetObject();
	m_color.setFromJsonObject(colObj);
}