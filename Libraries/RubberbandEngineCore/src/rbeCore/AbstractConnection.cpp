// @otlicense

/*
 *	File:		AbstractConnection.cpp
 *	Package:	rbeCore
 *
 *  Created on: September 10, 2021
 *	Author: Alexander Kuester
 *  Copyright (c) 2021 Alexander Kuester
 *	This file is part of the RubberbandEngine package.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

// RBE header
#include <rbeCore/AbstractConnection.h>

rbeCore::AbstractConnection::AbstractConnection(connectionType _type) 
	: m_type(_type), m_ignoreInHistory(false)
{}