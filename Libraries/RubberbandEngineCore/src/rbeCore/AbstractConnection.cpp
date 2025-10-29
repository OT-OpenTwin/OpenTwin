// @otlicense

// RBE header
#include <rbeCore/AbstractConnection.h>

rbeCore::AbstractConnection::AbstractConnection(connectionType _type) 
	: m_type(_type), m_ignoreInHistory(false)
{}