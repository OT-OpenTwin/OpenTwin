#include "OTBlockEditorAPI/BlockConfigurationGraphicsObject.h"
#include "OpenTwinCore/rJSONHelper.h"

#define JSOM_MEMBER_Orientation "Orientation"
#define JSOM_MEMBER_Margins "Margins"
#define JSOM_MEMBER_IsUserMoveable "IsUserMoveable"
#define JSON_MEMBER_HeightLimits "HeightLimits"
#define JSON_MEMBER_WidthLimits "WidthLimits"

ot::BlockConfigurationGraphicsObject::BlockConfigurationGraphicsObject(const std::string& _name)
	: BlockConfigurationObject(_name), m_orientation(OrientCenter), m_isUserMoveable(false)
{}

void ot::BlockConfigurationGraphicsObject::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	BlockConfigurationObject::addToJsonObject(_document, _object);

	OT_rJSON_createValueObject(marginsObj);
	m_margins.addToJsonObject(_document, marginsObj);

	OT_rJSON_createValueObject(heightObj);
	m_heightLimits.addToJsonObject(_document, heightObj);

	OT_rJSON_createValueObject(widthObj);
	m_widthLimits.addToJsonObject(_document, widthObj);


	ot::rJSON::add(_document, _object, JSOM_MEMBER_Orientation, toString(m_orientation));
	ot::rJSON::add(_document, _object, JSOM_MEMBER_Margins, marginsObj);
	ot::rJSON::add(_document, _object, JSOM_MEMBER_IsUserMoveable, m_isUserMoveable);
	ot::rJSON::add(_document, _object, JSON_MEMBER_HeightLimits, heightObj);
	ot::rJSON::add(_document, _object, JSON_MEMBER_WidthLimits, widthObj);
}

void ot::BlockConfigurationGraphicsObject::setFromJsonObject(OT_rJSON_val& _object) {
	BlockConfigurationObject::setFromJsonObject(_object);

	OT_rJSON_checkMember(_object, JSOM_MEMBER_Orientation, String);
	OT_rJSON_checkMember(_object, JSOM_MEMBER_Margins, Object);
	OT_rJSON_checkMember(_object, JSOM_MEMBER_IsUserMoveable, Bool);
	OT_rJSON_checkMember(_object, JSON_MEMBER_HeightLimits, Object);
	OT_rJSON_checkMember(_object, JSON_MEMBER_WidthLimits, Object);

	OT_rJSON_val marginsObj = _object[JSOM_MEMBER_Margins].GetObject();
	OT_rJSON_val heightObj = _object[JSON_MEMBER_HeightLimits].GetObject();
	OT_rJSON_val widthObj = _object[JSON_MEMBER_WidthLimits].GetObject();

	m_orientation = stringToOrientation(_object[JSOM_MEMBER_Orientation].GetString());
	m_margins.setFromJsonObject(marginsObj);
	m_isUserMoveable = _object[JSOM_MEMBER_IsUserMoveable].GetBool();
	m_heightLimits.setFromJsonObject(heightObj);
	m_widthLimits.setFromJsonObject(widthObj);
}

void ot::BlockConfigurationGraphicsObject::setMargins(double _top, double _right, double _bottom, double _left) {
	m_margins.set(_top, _right, _bottom, _left);
}
