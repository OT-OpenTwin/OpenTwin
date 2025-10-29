// @otlicense

/*
 *	File:		LineConnection.h
 *	Package:	rbeCore
 *
 *  Created on: September 10, 2021
 *	Author: Alexander Kuester
 *  Copyright (c) 2021 Alexander Kuester
 *	This file is part of the RubberbandEngine package.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#pragma once

// RBE header
#include <rbeCore/AbstractConnection.h>

namespace rbeCore {

	class AbstractPoint;

	class RBE_API_EXPORT LineConnection : public AbstractConnection {
	public:
		LineConnection();
		virtual ~LineConnection();

		// ###############################################################################

		// Base class functions

		virtual std::string debugInformation(const std::string& _prefix) override;

		virtual void addToJsonArray(RubberbandEngine * _engine, std::stringstream& _array, bool& _first) override;

		// ###############################################################################

		// Setter

		void setFrom(AbstractPoint * _from) { m_from = _from; }

		void setTo(AbstractPoint * _to) { m_to = _to; }

		// ###############################################################################

		// Getter

		AbstractPoint * from(void) const { return m_from; }

		AbstractPoint * to(void) const { return m_to; }

	private:

		AbstractPoint *		m_from;
		AbstractPoint *		m_to;

		LineConnection(LineConnection&) = delete;
		LineConnection& operator = (LineConnection&) = delete;
	};

}