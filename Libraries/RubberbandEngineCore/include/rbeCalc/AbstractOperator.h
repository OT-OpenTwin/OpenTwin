/*
 *	File:		AbstractOperator.h
 *	Package:	rbeCalc
 *
 *  Created on: September 07, 2021
 *	Author: Alexander Kuester
 *  Copyright (c) 2021 Alexander Kuester
 *	This file is part of the RubberbandEngine package.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#pragma once

// RBE header
#include <rbeCalc/AbstractCalculationItem.h>

namespace rbeCalc {

	class RBE_API_EXPORT AbstractOperator : public AbstractCalculationItem {
	public:
		AbstractOperator(AbstractCalculationItem * _lhv = nullptr, AbstractCalculationItem * _rhv = nullptr);
		virtual ~AbstractOperator();

		AbstractCalculationItem * left(void) { return m_lhv; }

		AbstractCalculationItem * right(void) { return m_rhv; }

		void replaceLeft(AbstractCalculationItem * _lhv);

		void replaceRight(AbstractCalculationItem * _rhv);

	protected:
		AbstractCalculationItem *		m_lhv;
		AbstractCalculationItem *		m_rhv;

	private:

		AbstractOperator(AbstractOperator&) = delete;
		AbstractOperator& operator = (AbstractOperator&) = delete;
	};

}