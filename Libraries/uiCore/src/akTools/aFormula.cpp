/*
 *	File:		aFormula.cpp
 *	Package:	akTools
 *
 *  Created on: February 02, 2022
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#include <akTools/aFormula.h>
#include <akCore/akCore.h>
#include <akCore/aAssert.h>

#define _USE_MATH_DEFINES

#include <math.h>

using namespace ak;

aFormula::aFormula() : m_topLevelNode(nullptr), m_tempId(0) {
	m_variables.insert_or_assign(std::string("pi"), M_PI);
}

aFormula::aFormula(const std::string& _formula, const std::list<std::pair<std::string, double>>& _variableNameValuePairs) 
	: m_topLevelNode(nullptr), m_tempId(0)
{
	m_variables.insert_or_assign(std::string("pi"), M_PI);
	addVariables(_variableNameValuePairs);
	aAssert(parse(_formula) == Success, "Failed to parse formula");
}

aFormula::~aFormula() {
	if (m_topLevelNode) {
		delete m_topLevelNode;
	}
}

// #############################################################################################

double aFormula::calculateFormula(const std::string& _formula, const std::list<std::pair<std::string, double>>& _variableNameValuePairs) {
	aFormula formula(_formula, _variableNameValuePairs);
	return formula.value();
}

// #############################################################################################

void aFormula::addVariable(const std::string& _variableName, double _variableValue) {
	m_variables.insert_or_assign(_variableName, _variableValue);
}

void aFormula::addVariable(const std::pair<std::string, double>& _variableNameValuePair) {
	m_variables.insert_or_assign(_variableNameValuePair.first, _variableNameValuePair.second);
}

void aFormula::addVariables(const std::list<std::pair<std::string, double>>& _variableNameValuePairs) {
	for (std::pair<std::string, double> entry : _variableNameValuePairs) {
		m_variables.insert_or_assign(entry.first, entry.second);
	}
}

aFormula::ParserErrorCode aFormula::parse(const std::string& _formula) {
	if (m_topLevelNode) {
		delete m_topLevelNode;
		m_topLevelNode = nullptr;
	}
	m_temp.clear();
	m_tempId = 0;

	try {
		m_topLevelNode = parseFormula(_formula);
	}
	catch (ParserErrorCode code) {
		m_temp.clear();
		return code;
	}
	catch (...) {
		m_temp.clear();
		return UnknownError;
	}
	return Success;
}

// #############################################################################################

double aFormula::variableValue(const std::string& _key) {
	auto it = m_variables.find(_key);
	if (it == m_variables.end()) {
		aAssert(0, "Key not found");
		return 0.;
	}
	return it->second;
}

double aFormula::value(void) const {
	if (m_topLevelNode) {
		return m_topLevelNode->value();
	}
	else {
		return 0.;
	}
}

// #############################################################################################

aAbstractFormulaItem * aFormula::parseFormula(const std::string& _formula) {
	using namespace std;
	if (_formula.find("(") != string::npos) {
		size_t indexL = _formula.find("(");
		size_t indexR = _formula.rfind(")");
		if (indexR == string::npos) { throw MissingClosingBracket; }
		
		bool found{ false };
		string key;
		while (!found) {
			key = "_" + to_string(m_tempId++) + "_";
			if (_formula.find(key) == string::npos) {
				if (m_temp.find(key) == m_temp.end()) {
					found = true;
				}
			}
		}
		
		m_temp.insert_or_assign(key, nullptr);
		m_temp.insert_or_assign(key, parseFormula(_formula.substr(indexL + 1, indexR - indexL - 1)));

		return parseFormula(_formula.substr(0, indexL) + key + _formula.substr(indexR + 1));
	}
	else if (_formula.find(")") != string::npos) { throw MissingOpeningBracket; }
	else if (_formula.find("+") != string::npos) {
		size_t ix = _formula.find("+");
		return new aFormulaOperatorAddition(parseFormula(_formula.substr(0, ix)), parseFormula(_formula.substr(ix + 1)));
	}
	else if (_formula.find("-") != string::npos) {
		size_t ix = _formula.find("-");
		return new aFormulaOperatorSubtraction(parseFormula(_formula.substr(0, ix)), parseFormula(_formula.substr(ix + 1)));
	}
	else if (_formula.find("*") != string::npos) {
		size_t ix = _formula.find("*");
		return new aFormulaOperatorMultiplication(parseFormula(_formula.substr(0, ix)), parseFormula(_formula.substr(ix + 1)));
	}
	else if (_formula.find("/") != string::npos) {
		size_t ix = _formula.find("/");
		return new aFormulaOperatorDivision(parseFormula(_formula.substr(0, ix)), parseFormula(_formula.substr(ix + 1)));
	}
	else if (_formula.find("^") != string::npos) {
		size_t ix = _formula.find("^");
		return new aFormulaOperatorPow(parseFormula(_formula.substr(0, ix)), parseFormula(_formula.substr(ix + 1)));
	}
	else {
		auto itV = m_variables.find(_formula);
		if (itV != m_variables.end()) {
			return new aFormulaValueItem(itV->second);
		}
		auto itT = m_temp.find(_formula);
		if (itT != m_temp.end()) {
			return itT->second;
		}
		bool fail{ false };
		double v = toNumber<double>(_formula, fail);
		if (fail) {
			aAssert(0, "Syntax error");
			throw SyntaxError;
		}
		return new aFormulaValueItem(v);
	}
}

// #####################################################################################################################################

// #####################################################################################################################################

// #####################################################################################################################################

aFormulaValueItem::aFormulaValueItem(double _value) : m_value(_value) {}

// #####################################################################################################################################

// #####################################################################################################################################

// #####################################################################################################################################

aAbstractFormulaOperator::aAbstractFormulaOperator(aAbstractFormulaItem * _left, aAbstractFormulaItem * _right) 
	: m_left(_left), m_right(_right) {}

aAbstractFormulaOperator::~aAbstractFormulaOperator() {
	if (m_left) { delete m_left; }
	if (m_right) { delete m_right; }
}

// #####################################################################################################################################

// #####################################################################################################################################

// #####################################################################################################################################

aFormulaOperatorAddition::aFormulaOperatorAddition(aAbstractFormulaItem * _left, aAbstractFormulaItem * _right) : aAbstractFormulaOperator(_left, _right) {}

aFormulaOperatorAddition::~aFormulaOperatorAddition() {}

double aFormulaOperatorAddition::value(void) const {
	double l = 0.0;
	double r = 0.0;
	if (m_left) { l = m_left->value(); }
	if (m_right) { r = m_right->value(); }
	return l + r;
}

// #####################################################################################################################################

// #####################################################################################################################################

// #####################################################################################################################################

aFormulaOperatorSubtraction::aFormulaOperatorSubtraction(aAbstractFormulaItem * _left, aAbstractFormulaItem * _right) : aAbstractFormulaOperator(_left, _right) {}

aFormulaOperatorSubtraction::~aFormulaOperatorSubtraction() {}

double aFormulaOperatorSubtraction::value(void) const {
	double l = 0.0;
	double r = 0.0;
	if (m_left) { l = m_left->value(); }
	if (m_right) { r = m_right->value(); }
	return l - r;
}

// #####################################################################################################################################

// #####################################################################################################################################

// #####################################################################################################################################

aFormulaOperatorMultiplication::aFormulaOperatorMultiplication(aAbstractFormulaItem * _left, aAbstractFormulaItem * _right) : aAbstractFormulaOperator(_left, _right) {}

aFormulaOperatorMultiplication::~aFormulaOperatorMultiplication() {}

double aFormulaOperatorMultiplication::value(void) const {
	double l = 0.0;
	double r = 0.0;
	if (m_left) { l = m_left->value(); }
	if (m_right) { r = m_right->value(); }
	return l * r;
}

// #####################################################################################################################################

// #####################################################################################################################################

// #####################################################################################################################################

aFormulaOperatorDivision::aFormulaOperatorDivision(aAbstractFormulaItem * _left, aAbstractFormulaItem * _right) : aAbstractFormulaOperator(_left, _right) {}

aFormulaOperatorDivision::~aFormulaOperatorDivision() {}

double aFormulaOperatorDivision::value(void) const {
	double l = 0.0;
	double r = 0.0;
	if (m_left) { l = m_left->value(); }
	if (m_right) { r = m_right->value(); }
	if (r == 0.0) {
		aAssert(0, "Division by 0");
		return 0.;
	}
	return l / r; }

// #####################################################################################################################################

// #####################################################################################################################################

// #####################################################################################################################################

aFormulaOperatorPow::aFormulaOperatorPow(aAbstractFormulaItem * _left, aAbstractFormulaItem * _right) : aAbstractFormulaOperator(_left, _right) {}

aFormulaOperatorPow::~aFormulaOperatorPow() {}

double aFormulaOperatorPow::value(void) const { return pow(m_left->value(), m_right->value()); }