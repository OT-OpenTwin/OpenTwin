// @otlicense

/*
 *	File:		AbstractPoint.cpp
 *	Package:	rbeCore
 *
 *  Created on: September 07, 2021
 *	Author: Alexander Kuester
 *  Copyright (c) 2021 Alexander Kuester
 *	This file is part of the RubberbandEngine package.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

// RBE header
#include <rbeCore/AbstractPoint.h>
#include <rbeCore/jsonMember.h>
#include <rbeCore/rbeAssert.h>

// C++ header
#include <math.h>

using namespace rbeCore;

coordinate_t AbstractPoint::distance(const AbstractPoint& _other, eAxisDistance _axis) const {
	coordinate_t d;
	switch (_axis)
	{
	case rbeCore::dAll:
		d = (coordinate_t)pow(pow(this->u() - _other.u(), 2) + pow(this->v() - _other.v(), 2) + pow(this->w() - _other.w(), 2), .5);
		break;
	case rbeCore::dU: d = this->u() - _other.u(); break;
	case rbeCore::dV: d = this->v() - _other.v(); break;
	case rbeCore::dW: d = this->w() - _other.w(); break;
	case rbeCore::dUV:
	{
		coordinate_t dx = this->u() - _other.u();
		coordinate_t dy = this->v() - _other.v();
		if (dx < 0) dx *= (-1);
		if (dy < 0) dy *= (-1);
		if (dx == 0 && dy == 0) { return 0; }
		d = (coordinate_t) sqrt(pow(dx, 2) + pow(dy, 2));
	}
	break;
	case rbeCore::dUW:
	{
		coordinate_t dx = this->u() - _other.u();
		coordinate_t dz = this->w() - _other.w();
		if (dx < 0.) dx *= (-1);
		if (dz < 0.) dz *= (-1);
		if (dx == 0 && dz == 0) { return 0.; }
		d = (coordinate_t) sqrt(pow(dx, 2) + pow(dz, 2));
	}
	break;
	case rbeCore::dVW:
	{
		coordinate_t dz = this->w() - _other.w();
		coordinate_t dy = this->v() - _other.v();
		if (dz < 0.) dz *= (-1);
		if (dy < 0.) dy *= (-1);
		if (dz == 0 && dy == 0) { return 0.; }
		d = (coordinate_t) sqrt(pow(dz, 2) + pow(dy, 2));
	}
	break;
	default:
		rbeAssert(0, "Unknown axis distance @AbstractPoint");
		d = 0;
		break;
	}
	if (d < 0.) d *= (-1);
	return d;
}

std::string AbstractPoint::debugInformation(const std::string& _prefix) {
	std::string ret{ "{\n" };
	ret.append(_prefix).append("\t\"ID\": ");
	ret.append(std::to_string(m_id));
	ret.append(",\n").append(_prefix).append("\t\"U\": ");
	ret.append(std::to_string(u()));
	ret.append(",\n").append(_prefix).append("\t\"V\": ");
	ret.append(std::to_string(v()));
	ret.append(",\n").append(_prefix).append("\t\"W\": ");
	ret.append(std::to_string(w()));
	ret.append(",\n").append(_prefix).append("}");
	return ret;

}

void AbstractPoint::addAsJsonObject(std::stringstream& _stream) const {
	_stream << "{\"" RBE_JSON_Point_ID "\":" << m_id << ",\"" RBE_JSON_Point_U "\":" << u() << ",\"" RBE_JSON_Point_V "\":" << v() <<
		",\"" RBE_JSON_Point_W "\":" << w() << "}";
}