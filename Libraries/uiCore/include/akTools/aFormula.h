/*
 *	File:		aFormula.h
 *	Package:	akTools
 *
 *  Created on: February 02, 2022
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#pragma once

#include <akCore/globalDataTypes.h>

#include <string>
#include <list>
#include <map>

namespace ak {

	class UICORE_API_EXPORT aAbstractFormulaItem {
	public:
		aAbstractFormulaItem() {}
		virtual ~aAbstractFormulaItem() {}

		virtual double value(void) const = 0;

	private:
		aAbstractFormulaItem(aAbstractFormulaItem&) = delete;
		aAbstractFormulaItem& operator = (aAbstractFormulaItem&) = delete;
	};

	// #####################################################################################################################################

	// #####################################################################################################################################

	// #####################################################################################################################################

	class UICORE_API_EXPORT aFormula {
	public:
		enum ParserErrorCode {
			Success,
			UnknownError,
			MissingOpeningBracket,
			MissingClosingBracket,
			SyntaxError
		};

		aFormula();
		aFormula(const std::string& _formula, const std::list<std::pair<std::string, double>>& _variableNameValuePairs = std::list<std::pair<std::string, double>>());
		virtual ~aFormula();

		// #############################################################################################

		static double calculateFormula(const std::string& _formula, const std::list<std::pair<std::string, double>>& _variableNameValuePairs = std::list<std::pair<std::string, double>>());

		// #############################################################################################

		void addVariable(const std::string& _variableName, double _variableValue);
		void addVariable(const std::pair<std::string, double>& _variableNameValuePair);
		void addVariables(const std::list<std::pair<std::string, double>>& _variableNameValuePairs);

		ParserErrorCode parse(const std::string& _formula);

		// #############################################################################################

		double variableValue(const std::string& _key);

		double value(void) const;

	private:

		aAbstractFormulaItem * parseFormula(const std::string& _formula);

		aAbstractFormulaItem *							m_topLevelNode;
		std::map<std::string, double>					m_variables;

		std::map<std::string, aAbstractFormulaItem *>	m_temp;
		size_t											m_tempId;

		aFormula(aFormula&) = delete;
		aFormula& operator = (aFormula&) = delete;
	};

	// #####################################################################################################################################

	// #####################################################################################################################################

	// #####################################################################################################################################

	class UICORE_API_EXPORT aFormulaValueItem : public aAbstractFormulaItem {
	public:
		aFormulaValueItem(double _value = 0.0);
		virtual ~aFormulaValueItem() {}

		void setValue(double _value) { m_value = _value; }
		virtual double value(void) const { return m_value; }

	private:
		double			m_value;

		aFormulaValueItem(aFormulaValueItem&) = delete;
		aFormulaValueItem& operator = (aFormulaValueItem&) = delete;
	};

	// #####################################################################################################################################

	// #####################################################################################################################################

	// #####################################################################################################################################

	class UICORE_API_EXPORT aAbstractFormulaOperator : public aAbstractFormulaItem {
	public:
		aAbstractFormulaOperator(aAbstractFormulaItem * _left = (aAbstractFormulaItem *)nullptr, aAbstractFormulaItem * _right = (aAbstractFormulaItem *)nullptr);
		virtual ~aAbstractFormulaOperator();

		void setLeft(aAbstractFormulaItem * _left) { m_left = _left; }
		void setRight(aAbstractFormulaItem * _right) { m_right = _right; }

		aAbstractFormulaItem * left(void) { return m_left; }
		aAbstractFormulaItem * right(void) { return m_right; }

	protected:
		aAbstractFormulaItem *		m_left;
		aAbstractFormulaItem *		m_right;

	private:
		aAbstractFormulaOperator(aAbstractFormulaOperator&) = delete;
		aAbstractFormulaOperator& operator = (aAbstractFormulaOperator&) = delete;
	};

	// #####################################################################################################################################

	// #####################################################################################################################################

	// #####################################################################################################################################

	class UICORE_API_EXPORT aFormulaOperatorAddition : public aAbstractFormulaOperator {
	public:
		aFormulaOperatorAddition(aAbstractFormulaItem * _left = (aAbstractFormulaItem *)nullptr, aAbstractFormulaItem * _right = (aAbstractFormulaItem *)nullptr);
		virtual ~aFormulaOperatorAddition();

		virtual double value(void) const;

	private:
		aFormulaOperatorAddition(aFormulaOperatorAddition&) = delete;
		aFormulaOperatorAddition& operator = (aFormulaOperatorAddition&) = delete;
	};

	// #####################################################################################################################################

	// #####################################################################################################################################

	// #####################################################################################################################################

	class UICORE_API_EXPORT aFormulaOperatorSubtraction : public aAbstractFormulaOperator {
	public:
		aFormulaOperatorSubtraction(aAbstractFormulaItem * _left = (aAbstractFormulaItem *)nullptr, aAbstractFormulaItem * _right = (aAbstractFormulaItem *)nullptr);
		virtual ~aFormulaOperatorSubtraction();

		virtual double value(void) const;

	private:
		aFormulaOperatorSubtraction(aFormulaOperatorSubtraction&) = delete;
		aFormulaOperatorSubtraction& operator = (aFormulaOperatorSubtraction&) = delete;
	};

	// #####################################################################################################################################

	// #####################################################################################################################################

	// #####################################################################################################################################

	class UICORE_API_EXPORT aFormulaOperatorMultiplication : public aAbstractFormulaOperator {
	public:
		aFormulaOperatorMultiplication(aAbstractFormulaItem * _left = (aAbstractFormulaItem *)nullptr, aAbstractFormulaItem * _right = (aAbstractFormulaItem *)nullptr);
		virtual ~aFormulaOperatorMultiplication();

		virtual double value(void) const;

	private:
		aFormulaOperatorMultiplication(aFormulaOperatorMultiplication&) = delete;
		aFormulaOperatorMultiplication& operator = (aFormulaOperatorMultiplication&) = delete;
	};

	// #####################################################################################################################################

	// #####################################################################################################################################

	// #####################################################################################################################################

	class UICORE_API_EXPORT aFormulaOperatorDivision : public aAbstractFormulaOperator {
	public:
		aFormulaOperatorDivision(aAbstractFormulaItem * _left = (aAbstractFormulaItem *)nullptr, aAbstractFormulaItem * _right = (aAbstractFormulaItem *)nullptr);
		virtual ~aFormulaOperatorDivision();

		virtual double value(void) const;

	private:
		aFormulaOperatorDivision(aFormulaOperatorDivision&) = delete;
		aFormulaOperatorDivision& operator = (aFormulaOperatorDivision&) = delete;
	};

	// #####################################################################################################################################

	// #####################################################################################################################################

	// #####################################################################################################################################

	class UICORE_API_EXPORT aFormulaOperatorPow : public aAbstractFormulaOperator {
	public:
		aFormulaOperatorPow(aAbstractFormulaItem * _left = (aAbstractFormulaItem *)nullptr, aAbstractFormulaItem * _right = (aAbstractFormulaItem *)nullptr);
		virtual ~aFormulaOperatorPow();

		virtual double value(void) const;

	private:
		aFormulaOperatorPow(aFormulaOperatorPow&) = delete;
		aFormulaOperatorPow& operator = (aFormulaOperatorPow&) = delete;
	};
}