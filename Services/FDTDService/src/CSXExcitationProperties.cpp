//! @file CSXExcitationProperties.cpp
//! @author Alexandros McCray (alexm-dev)
//! @date 24.10.2025
// ###########################################################################################################################################################################################################################################################################################################################

#include "CSXExcitationProperties.h"

CSXExcitationProperties::CSXExcitationProperties() 
{
}

CSXExcitationProperties::~CSXExcitationProperties()
{
}

void CSXExcitationProperties::setPrimitives1(double _x, double _y, double _z) {
	m_primitives1_X = _x;
	m_primitives1_Y = _y;
	m_primitives1_Z = _z;
}

void CSXExcitationProperties::setPrimitives2(double _x, double _y, double _z) {
	m_primitives2_X = _x;
	m_primitives2_Y = _y;
	m_primitives2_Z = _z;
}

void CSXExcitationProperties::setExciteDirection(bool _exciteX, bool _exciteY, bool _exciteZ) {
	bool flags[3] = { _exciteX, _exciteY, _exciteZ };
	m_excite = "";
	for (int i = 0; i < 3; ++i) {
		if (flags[i]) {
			m_excite += "1";
		}
		else {
			m_excite += "0";
		}
		if (i < 2) {
			m_excite += ",";
		}
	}
}

tinyxml2::XMLElement* CSXExcitationProperties::writeCSXExciteProperties(tinyxml2::XMLElement& _parentElement) const {
	auto excitationProperty = _parentElement.GetDocument()->NewElement("Excitation");
	excitationProperty->SetAttribute("Excite", m_excite.c_str());
	excitationProperty->SetAttribute("Name", m_name.c_str());
	excitationProperty->SetAttribute("Type", m_type);

	auto weightElement = _parentElement.GetDocument()->NewElement("Weight");
	weightElement->SetAttribute("X", m_weightX.c_str());
	weightElement->SetAttribute("Y", m_weightY.c_str());
	weightElement->SetAttribute("Z", m_weightZ.c_str());
	excitationProperty->InsertEndChild(weightElement);

	auto primitivesElement = _parentElement.GetDocument()->NewElement("Primitives");
	auto boxElement = _parentElement.GetDocument()->NewElement("Box");
	boxElement->SetAttribute("Priority", m_boxPriority);

	auto primitive1Element = _parentElement.GetDocument()->NewElement("P1");
	primitive1Element->SetAttribute("X", m_primitives1_X);
	primitive1Element->SetAttribute("Y", m_primitives1_Y);
	primitive1Element->SetAttribute("Z", m_primitives1_Z);

	auto primitive2Element = _parentElement.GetDocument()->NewElement("P2");
	primitive2Element->SetAttribute("X", m_primitives2_X);
	primitive2Element->SetAttribute("Y", m_primitives2_Y);
	primitive2Element->SetAttribute("Z", m_primitives2_Z);

	boxElement->InsertEndChild(primitive1Element);
	boxElement->InsertEndChild(primitive2Element);
	primitivesElement->InsertEndChild(boxElement);
	excitationProperty->InsertEndChild(primitivesElement);
	_parentElement.InsertEndChild(excitationProperty);
	return excitationProperty;
}