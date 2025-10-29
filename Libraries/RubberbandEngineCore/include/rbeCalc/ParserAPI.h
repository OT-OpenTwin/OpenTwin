// @otlicense

/*
 *	File:		FormulaParser.h
 *	Package:	rbeCalc
 *
 *  Created on: September 08, 2021
 *	Author: Alexander Kuester
 *  Copyright (c) 2021 Alexander Kuester
 *	This file is part of the RubberbandEngine package.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#pragma once

// RBE header
#include <rbeCore/dataTypes.h>

// C++ header
#include <string>

namespace rbeCore {
	class RubberbandEngine;
	class AbstractPoint;
	class Step;
}

namespace rbeCalc {

	class AbstractCalculationItem;

	namespace ParserAPI {

		RBE_API_EXPORT rbeCalc::AbstractCalculationItem * parseFormula(rbeCore::RubberbandEngine * _engine, rbeCore::Step * _step, const std::string& _string);
	
		RBE_API_EXPORT rbeCore::AbstractPoint * parsePoint(rbeCore::RubberbandEngine * _engine, rbeCore::Step * _step, const std::string& _str, rbeCore::eAxisDistance& _distance);

	}
}