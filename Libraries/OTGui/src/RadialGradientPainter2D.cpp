//! @file RadialGradientPainter2D.cpp
//! @author Alexander Kuester (alexk95)
//! @date November 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/Painter2DFactory.h"
#include "OTGui/RadialGradientPainter2D.h"

#define OT_JSON_MEMBER_CenterPoint "Center.Point"
#define OT_JSON_MEMBER_CenterRadius "Center.Radius"
#define OT_JSON_MEMBER_FocalPoint "Focal.Point"
#define OT_JSON_MEMBER_FocalRadius "Focal.Radius"
#define OT_JSON_MEMBER_FocalIsSet "Focal.IsSet"

static ot::Painter2DFactoryRegistrar<ot::RadialGradientPainter2D> radGradCfgRegistrar(OT_FactoryKey_RadialGradientPainter2D);

ot::RadialGradientPainter2D::RadialGradientPainter2D()
	: m_centerRadius(1.), m_focalRadius(1.), m_focalSet(false)
{}

ot::RadialGradientPainter2D::~RadialGradientPainter2D() {}

void ot::RadialGradientPainter2D::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	ot::GradientPainter2D::addToJsonObject(_object, _allocator);

	JsonObject focalObj;
	JsonObject centerObj;
	m_focal.addToJsonObject(focalObj, _allocator);
	m_center.addToJsonObject(centerObj, _allocator);
	_object.AddMember(OT_JSON_MEMBER_FocalPoint, focalObj, _allocator);
	_object.AddMember(OT_JSON_MEMBER_FocalIsSet, m_focalSet, _allocator);
	_object.AddMember(OT_JSON_MEMBER_CenterPoint, centerObj, _allocator);
	_object.AddMember(OT_JSON_MEMBER_FocalRadius, m_focalRadius, _allocator);
	_object.AddMember(OT_JSON_MEMBER_CenterRadius, m_centerRadius, _allocator);
}

void ot::RadialGradientPainter2D::setFromJsonObject(const ConstJsonObject& _object) {
	ot::GradientPainter2D::setFromJsonObject(_object);
	m_center.setFromJsonObject(json::getObject(_object, OT_JSON_MEMBER_CenterPoint));
	m_focal.setFromJsonObject(json::getObject(_object, OT_JSON_MEMBER_FocalPoint));
	m_centerRadius = json::getDouble(_object, OT_JSON_MEMBER_CenterRadius);
	m_focalSet = json::getBool(_object, OT_JSON_MEMBER_FocalIsSet);
	m_focalRadius = json::getDouble(_object, OT_JSON_MEMBER_FocalRadius);
}

void ot::RadialGradientPainter2D::setFocalPoint(const ot::Point2DD& _focal) {
	m_focal = _focal;
	m_focalSet = true;
}

std::string ot::RadialGradientPainter2D::generateQss(void) const {
	std::string ret = "qradialgradient(cx: " + std::to_string(m_center.x()) +
						", cy: " + std::to_string(m_center.y()) +
						", radius: " + std::to_string(m_centerRadius);
	if (m_focalSet) {
		ret.append(", fx: " + std::to_string(m_focal.x()) +
			", fy: " + std::to_string(m_focal.y()) +
			", fradius: " + std::to_string(m_focalRadius)
		);
	}
	
	this->addStopsAndSpreadToQss(ret);
	ret.append(")");

	return ret;
}

bool ot::RadialGradientPainter2D::isEqualTo(const Painter2D* _other) const {
	if (!GradientPainter2D::isEqualTo(_other)) return false;

	const RadialGradientPainter2D* otherPainter = dynamic_cast<const RadialGradientPainter2D*>(_other);
	if (!otherPainter) return false;

	if (m_center != otherPainter->getCenterPoint()) return false;
	if (m_centerRadius != otherPainter->getCenterRadius()) return false;
	if (m_focal != otherPainter->getFocalPoint()) return false;
	if (m_focalRadius != otherPainter->getFocalRadius()) return false;
	return m_focalSet == otherPainter->isFocalPointSet();
}
