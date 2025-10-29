// @otlicense

/*
 *	File:		HistoryConnection.cpp
 *	Package:	rbeCore
 *
 *  Created on: September 11, 2021
 *	Author: Alexander Kuester
 *  Copyright (c) 2021 Alexander Kuester
 *	This file is part of the RubberbandEngine package.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

// RBE header
#include <rbeCore/HistoryConnection.h>
#include <rbeCore/Step.h>
#include <rbeCore/jsonMember.h>

rbeCore::HistoryConnection::HistoryConnection(Step * _ref)
	: AbstractConnection(ctHistory), m_ref(_ref)
{}

rbeCore::HistoryConnection::~HistoryConnection() {}

// ###############################################################################

// Base class functions

std::string rbeCore::HistoryConnection::debugInformation(const std::string& _prefix) {
	std::string ret{ "{\n" };
	ret.append(_prefix).append("\t\"" RBE_JSON_CONNECTION_Type "\": \"" RBE_JSON_VALUE_ConnectionType_History "\",\n");
	ret.append(_prefix).append("\t\"" RBE_JSON_STEP_Step "\": ");
	if (m_ref) {
		ret.append(std::to_string(m_ref->id())).append("\n");
	}
	else {
		ret.append("NULL,\n");
	}
	ret.append(_prefix).append("}");

	return ret;
}

void rbeCore::HistoryConnection::addToJsonArray(RubberbandEngine * _engine, std::stringstream& _array, bool& _first) {
	if (m_ref) {
		m_ref->addConnectionsToJsonArray(_engine, _array, _first, true);
	}
}
