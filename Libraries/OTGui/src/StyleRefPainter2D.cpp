//! @file StyleRefPainter2D.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/StyleRefPainter2D.h"
#include "OTGui/Painter2DFactory.h"

static ot::Painter2DFactoryRegistrar<ot::StyleRefPainter2D> fillCfgRegistrar(OT_FactoryKey_StyleRefPainter2D);

ot::StyleRefPainter2D::StyleRefPainter2D(ColorStyleValueEntry _referenceKey) : m_reference(_referenceKey) {}

ot::StyleRefPainter2D::~StyleRefPainter2D() {}

void ot::StyleRefPainter2D::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	ot::Painter2D::addToJsonObject(_object, _allocator);
	_object.AddMember("Ref", JsonString(toString(m_reference), _allocator), _allocator);
}

void ot::StyleRefPainter2D::setFromJsonObject(const ConstJsonObject& _object) {
	ot::Painter2D::setFromJsonObject(_object);
	m_reference = stringToColorStyleValueEntry(json::getString(_object, "Ref"));
}

std::string ot::StyleRefPainter2D::generateQss(void) const {
	OT_LOG_E("StyleRefPainter2D can not be used to generate Qss.");
	return "/*<referencing style value \"" + toString(m_reference) + "\">*/";
}

ot::Color ot::StyleRefPainter2D::getDefaultColor(void) const {
	return Color();
}

bool ot::StyleRefPainter2D::isEqualTo(const Painter2D* _other) const {
	const StyleRefPainter2D* otherPainter = dynamic_cast<const StyleRefPainter2D*>(_other);
	if (!otherPainter) return false;

	return m_reference == otherPainter->getReferenceKey();
}