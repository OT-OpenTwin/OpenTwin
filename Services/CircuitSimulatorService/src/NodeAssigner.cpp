// @otlicense
// File: NodeAssigner.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

// Service Header
#include "NodeAssigner.h"
#include "Application.h"

// Open Twin Header
#include "OTCore/Logging/Logger.h"

void NodeAssigner::reset()
{
	m_currentNodeNumber = 1;
	m_connectionNodeNumbers.clear();
}

void NodeAssigner::assignNodeNumbers(
	std::map<ot::UID, ot::UIDList>& _connectionBlockMap,
	Circuit& _circuit,
	std::map<ot::UID, std::shared_ptr<ot::EntityBlockConnection>>& _allConnectionEntities,
	std::map<ot::UID, std::shared_ptr<ot::EntityBlock>>& _allEntitiesByBlockID,
	const std::string& _editorname)
{
	// First check for GND elements
	auto vectorGND = _circuit.getMapOfEntityBlcks().find("EntityBlockCircuitGND");
	if (vectorGND != _circuit.getMapOfEntityBlcks().end())
	{
		std::set<ot::UID> visitedElements;

		for (auto GNDElement : vectorGND->second)
		{
			ot::UID elementUID = GNDElement->getEntityID();
			int counter = 0;
			traverseFromGND(_connectionBlockMap, GNDElement->getClassName(), counter, elementUID, elementUID, _allConnectionEntities, _allEntitiesByBlockID, _editorname, _circuit, visitedElements);
		}
	}
	else
	{
		auto vectorVoltageSource = _circuit.getMapOfEntityBlcks().find("EntityBlockCircuitVoltageSource");
		if (vectorVoltageSource == _circuit.getMapOfEntityBlcks().end())
		{
			OT_LOG_E("No VoltageSource found at connection Algorithm");
			return;
		}
		std::set<ot::UID> visitedElements;

		for (auto voltageSource : vectorVoltageSource->second)
		{
			ot::UID elementUID = voltageSource->getEntityID();
			int counter = 0;

			// First set the GND connections of the VoltageSource
			setNodeNumbersOfVoltageSource(_connectionBlockMap, voltageSource->getClassName(), counter, elementUID, elementUID, _allConnectionEntities, _allEntitiesByBlockID, _editorname, _circuit, visitedElements);

			// Now go through all the other connections
			traverseFromVoltageSource(_connectionBlockMap, voltageSource->getClassName(), counter, elementUID, elementUID, _allConnectionEntities, _allEntitiesByBlockID, _editorname, _circuit, visitedElements);
		}
	}
}

void NodeAssigner::traverseFromGND(
	std::map<ot::UID, ot::UIDList>& _connectionBlockMap,
	std::string _startingElement, int _counter,
	ot::UID _startingElementUID, ot::UID _elementUID,
	std::map<ot::UID, std::shared_ptr<ot::EntityBlockConnection>> _allConnectionEntities,
	std::map<ot::UID, std::shared_ptr<ot::EntityBlock>>& _allEntitiesByBlockID,
	const std::string& _editorname, Circuit& _circuit,
	std::set<ot::UID>& _visitedElements)
{
	_counter++;

	auto appInstance = Application::instance();
	auto element = getEntityBlock(_allEntitiesByBlockID, _elementUID);
	if (element == nullptr)
	{
		return;
	}
	auto connections = getConnections(_connectionBlockMap, _elementUID);
	if (connections.empty())
	{
		return;
	}

	// Check if Element already exists
	if (isVisited(_visitedElements, _elementUID))
	{
		return;
	}

	for (auto connection : connections)
	{
		Connection myConn = createConnection(_allConnectionEntities, connection);

		// I always start with GND Element and give the Connection the nodeNumber 0
		if (_counter == 1 && _startingElement == "EntityBlockCircuitGND")
		{
			if (isGNDConnection(myConn.getOriginConnectable()) ||
				isGNDConnection(myConn.getDestinationConnectable()))
			{
				myConn.setNodeNumber("0");
				m_connectionNodeNumbers[{ myConn.getDestinationUid(), myConn.getDestinationConnectable() }] = myConn.getNodeNumber();
				m_connectionNodeNumbers[{ myConn.getOriginUid(), myConn.getOriginConnectable() }] = myConn.getNodeNumber();
				_circuit.addConnection(myConn.getOriginConnectable(), myConn.getOriginUid(), myConn);
				_circuit.addConnection(myConn.getDestinationConnectable(), myConn.getDestinationUid(), myConn);
			}
		}

		// Check the case if the connection is connected to a connector
		if (appInstance->extractStringAfterDelimiter(myConn.getDestinationConnectable(), '/', 2).find("Connector") != std::string::npos && myConn.getDestinationUid() != element->getEntityID() ||
			appInstance->extractStringAfterDelimiter(myConn.getOriginConnectable(), '/', 2).find("Connector") != std::string::npos && myConn.getOriginUid() != element->getEntityID())
		{
			ot::UID nextElementUID;
			if (myConn.getOriginUid() == _elementUID)
			{
				nextElementUID = myConn.getDestinationUid();
			}
			else
			{
				nextElementUID = myConn.getOriginUid();
			}

			handleWithConnectors(_connectionBlockMap, nextElementUID, _allConnectionEntities, _allEntitiesByBlockID, _editorname, _circuit, _visitedElements);

			traverseFromGND(_connectionBlockMap, _startingElement, _counter, _startingElementUID, nextElementUID, _allConnectionEntities, _allEntitiesByBlockID, _editorname, _circuit, _visitedElements);
		}
		else
		{
			// Here i check if connection already exists
			if (isVisited(_visitedElements, connection))
			{
				ot::UID nextElementUID;
				if (myConn.getOriginUid() == _elementUID)
				{
					nextElementUID = myConn.getDestinationUid();
				}
				else
				{
					nextElementUID = myConn.getOriginUid();
				}

				traverseFromGND(_connectionBlockMap, _startingElement, _counter, _startingElementUID, nextElementUID, _allConnectionEntities, _allEntitiesByBlockID, _editorname, _circuit, _visitedElements);
			}
			else
			{
				assignNodeNumber(myConn);

				_circuit.addConnection(myConn.getOriginConnectable(), myConn.getOriginUid(), myConn);
				_circuit.addConnection(myConn.getDestinationConnectable(), myConn.getDestinationUid(), myConn);

				// Recursive call to explore the next element
				ot::UID nextElementUID;
				if (myConn.getOriginUid() == _elementUID)
				{
					nextElementUID = myConn.getDestinationUid();
				}
				else
				{
					nextElementUID = myConn.getOriginUid();
				}
				traverseFromGND(_connectionBlockMap, _startingElement, _counter, _startingElementUID, nextElementUID, _allConnectionEntities, _allEntitiesByBlockID, _editorname, _circuit, _visitedElements);
			}
		}
	}
}

void NodeAssigner::traverseFromVoltageSource(
	std::map<ot::UID, ot::UIDList>& _connectionBlockMap,
	std::string _startingElement, int _counter,
	ot::UID _startingElementUID, ot::UID _elementUID,
	std::map<ot::UID, std::shared_ptr<ot::EntityBlockConnection>> _allConnectionEntities,
	std::map<ot::UID, std::shared_ptr<ot::EntityBlock>>& _allEntitiesByBlockID,
	const std::string& _editorname, Circuit& _circuit,
	std::set<ot::UID>& _visitedElements)
{
	_counter++;

	auto appInstance = Application::instance();
	auto element = getEntityBlock(_allEntitiesByBlockID, _elementUID);
	if (element == nullptr)
	{
		return;
	}
	auto connections = getConnections(_connectionBlockMap, _elementUID);
	if (connections.empty())
	{
		return;
	}

	// Check if Element already exists
	if (isVisited(_visitedElements, _elementUID))
	{
		return;
	}

	for (auto connection : connections)
	{
		Connection myConn = createConnection(_allConnectionEntities, connection);

		if (_counter == 1 && _startingElement == "EntityBlockCircuitVoltageSource")
		{
			if (isGndVoltageSourceConnection(myConn.getOriginConnectable(), _startingElementUID, myConn.getOriginUid()) ||
				isGndVoltageSourceConnection(myConn.getDestinationConnectable(), _startingElementUID, myConn.getDestinationUid()))
			{
				myConn.setNodeNumber("0");
				m_connectionNodeNumbers[{ myConn.getDestinationUid(), myConn.getDestinationConnectable() }] = myConn.getNodeNumber();
				m_connectionNodeNumbers[{ myConn.getOriginUid(), myConn.getOriginConnectable() }] = myConn.getNodeNumber();
				_circuit.addConnection(myConn.getOriginConnectable(), myConn.getOriginUid(), myConn);
				_circuit.addConnection(myConn.getDestinationConnectable(), myConn.getDestinationUid(), myConn);
			}
		}

		// Check the case if the connection is connected to a connector
		if (appInstance->extractStringAfterDelimiter(myConn.getDestinationConnectable(), '/', 2).find("Connector") != std::string::npos && myConn.getDestinationUid() != element->getEntityID() ||
			appInstance->extractStringAfterDelimiter(myConn.getOriginConnectable(), '/', 2).find("Connector") != std::string::npos && myConn.getOriginUid() != element->getEntityID())
		{
			ot::UID nextElementUID;
			if (myConn.getOriginUid() == _elementUID)
			{
				nextElementUID = myConn.getDestinationUid();
			}
			else
			{
				nextElementUID = myConn.getOriginUid();
			}

			handleWithConnectors(_connectionBlockMap, nextElementUID, _allConnectionEntities, _allEntitiesByBlockID, _editorname, _circuit, _visitedElements);

			traverseFromGND(_connectionBlockMap, _startingElement, _counter, _startingElementUID, nextElementUID, _allConnectionEntities, _allEntitiesByBlockID, _editorname, _circuit, _visitedElements);
		}
		else
		{
			if (isVisited(_visitedElements, connection))
			{
				ot::UID nextElementUID;
				if (myConn.getOriginUid() == _elementUID)
				{
					nextElementUID = myConn.getDestinationUid();
				}
				else
				{
					nextElementUID = myConn.getOriginUid();
				}

				traverseFromVoltageSource(_connectionBlockMap, _startingElement, _counter, _startingElementUID, nextElementUID, _allConnectionEntities, _allEntitiesByBlockID, _editorname, _circuit, _visitedElements);
			}
			else
			{
				assignNodeNumber(myConn);

				_circuit.addConnection(myConn.getOriginConnectable(), myConn.getOriginUid(), myConn);
				_circuit.addConnection(myConn.getDestinationConnectable(), myConn.getDestinationUid(), myConn);

				// Recursive call to explore the next element
				ot::UID nextElementUID;
				if (myConn.getOriginUid() == _elementUID)
				{
					nextElementUID = myConn.getDestinationUid();
				}
				else
				{
					nextElementUID = myConn.getOriginUid();
				}
				traverseFromVoltageSource(_connectionBlockMap, _startingElement, _counter, _startingElementUID, nextElementUID, _allConnectionEntities, _allEntitiesByBlockID, _editorname, _circuit, _visitedElements);
			}
		}
	}
}

void NodeAssigner::handleWithConnectors(
	std::map<ot::UID, ot::UIDList>& _connectionBlockMap,
	ot::UID _elementUID,
	std::map<ot::UID, std::shared_ptr<ot::EntityBlockConnection>> _allConnectionEntities,
	std::map<ot::UID, std::shared_ptr<ot::EntityBlock>>& _allEntitiesByBlockID,
	const std::string& _editorname, Circuit& _circuit,
	std::set<ot::UID>& _visitedElements)
{
	auto appInstance = Application::instance();
	auto element = getEntityBlock(_allEntitiesByBlockID, _elementUID);
	if (element == nullptr)
	{
		return;
	}
	auto connections = getConnections(_connectionBlockMap, _elementUID);
	if (connections.empty())
	{
		return;
	}

	for (auto connection : connections)
	{
		Connection myConn = createConnection(_allConnectionEntities, connection);
		if (isVisited(_visitedElements, connection))
		{
			continue;
		}

		assignNodeNumber(myConn);

		_circuit.addConnection(myConn.getOriginConnectable(), myConn.getOriginUid(), myConn);
		_circuit.addConnection(myConn.getDestinationConnectable(), myConn.getDestinationUid(), myConn);

		if (appInstance->extractStringAfterDelimiter(myConn.getDestinationConnectable(), '/', 2).find("Connector") != std::string::npos && myConn.getDestinationUid() != element->getEntityID())
		{
			handleWithConnectors(_connectionBlockMap, myConn.getDestinationUid(), _allConnectionEntities, _allEntitiesByBlockID, _editorname, _circuit, _visitedElements);
		}
		else if (appInstance->extractStringAfterDelimiter(myConn.getOriginConnectable(), '/', 2).find("Connector") != std::string::npos && myConn.getOriginUid() != element->getEntityID())
		{
			handleWithConnectors(_connectionBlockMap, myConn.getOriginUid(), _allConnectionEntities, _allEntitiesByBlockID, _editorname, _circuit, _visitedElements);
		}
	}
}

void NodeAssigner::assignNodeNumber(Connection& _connection)
{
	if (isGNDConnection(_connection.getOriginConnectable()) ||
		isGNDConnection(_connection.getDestinationConnectable()))
	{
		auto connectionWithNodeNumber = m_connectionNodeNumbers.find({ _connection.getDestinationUid(), _connection.getDestinationConnectable() });
		if (connectionWithNodeNumber != m_connectionNodeNumbers.end())
		{
			_connection.setNodeNumber(connectionWithNodeNumber->second);
			m_connectionNodeNumbers[{_connection.getOriginUid(), _connection.getOriginConnectable()}] = _connection.getNodeNumber();
		}
		else
		{
			connectionWithNodeNumber = m_connectionNodeNumbers.find({ _connection.getOriginUid(), _connection.getOriginConnectable() });
			if (connectionWithNodeNumber != m_connectionNodeNumbers.end())
			{
				_connection.setNodeNumber(connectionWithNodeNumber->second);
				m_connectionNodeNumbers[{_connection.getDestinationUid(), _connection.getDestinationConnectable()}] = _connection.getNodeNumber();
			}
			else
			{
				_connection.setNodeNumber("0");
				m_connectionNodeNumbers[{ _connection.getDestinationUid(), _connection.getDestinationConnectable() }] = _connection.getNodeNumber();
				m_connectionNodeNumbers[{ _connection.getOriginUid(), _connection.getOriginConnectable() }] = _connection.getNodeNumber();
			}
		}
	}
	else
	{
		auto connectionWithNodeNumber = m_connectionNodeNumbers.find({ _connection.getDestinationUid(), _connection.getDestinationConnectable() });
		if (connectionWithNodeNumber != m_connectionNodeNumbers.end())
		{
			_connection.setNodeNumber(connectionWithNodeNumber->second);
			m_connectionNodeNumbers[{_connection.getOriginUid(), _connection.getOriginConnectable()}] = _connection.getNodeNumber();
		}
		else
		{
			connectionWithNodeNumber = m_connectionNodeNumbers.find({ _connection.getOriginUid(), _connection.getOriginConnectable() });
			if (connectionWithNodeNumber != m_connectionNodeNumbers.end())
			{
				_connection.setNodeNumber(connectionWithNodeNumber->second);
				m_connectionNodeNumbers[{_connection.getDestinationUid(), _connection.getDestinationConnectable()}] = _connection.getNodeNumber();
			}
			else
			{
				_connection.setNodeNumber(std::to_string(m_currentNodeNumber++));
				m_connectionNodeNumbers[{ _connection.getDestinationUid(), _connection.getDestinationConnectable() }] = _connection.getNodeNumber();
				m_connectionNodeNumbers[{ _connection.getOriginUid(), _connection.getOriginConnectable() }] = _connection.getNodeNumber();
			}
		}
	}
}

void NodeAssigner::assignNodeNumberForGndVoltageSource(Connection& _connection, ot::UID _startingElementUID)
{
	if (isGndVoltageSourceConnection(_connection.getOriginConnectable(), _startingElementUID, _connection.getOriginUid()) ||
		isGndVoltageSourceConnection(_connection.getDestinationConnectable(), _startingElementUID, _connection.getDestinationUid()))
	{
		auto connectionWithNodeNumber = m_connectionNodeNumbers.find({ _connection.getDestinationUid(), _connection.getDestinationConnectable() });
		if (connectionWithNodeNumber != m_connectionNodeNumbers.end())
		{
			_connection.setNodeNumber(connectionWithNodeNumber->second);
			m_connectionNodeNumbers[{_connection.getOriginUid(), _connection.getOriginConnectable()}] = _connection.getNodeNumber();
		}
		else
		{
			connectionWithNodeNumber = m_connectionNodeNumbers.find({ _connection.getOriginUid(), _connection.getOriginConnectable() });
			if (connectionWithNodeNumber != m_connectionNodeNumbers.end())
			{
				_connection.setNodeNumber(connectionWithNodeNumber->second);
				m_connectionNodeNumbers[{_connection.getDestinationUid(), _connection.getDestinationConnectable()}] = _connection.getNodeNumber();
			}
			else
			{
				_connection.setNodeNumber("0");
				m_connectionNodeNumbers[{ _connection.getDestinationUid(), _connection.getDestinationConnectable() }] = _connection.getNodeNumber();
				m_connectionNodeNumbers[{ _connection.getOriginUid(), _connection.getOriginConnectable() }] = _connection.getNodeNumber();
			}
		}
	}
	else
	{
		auto connectionWithNodeNumber = m_connectionNodeNumbers.find({ _connection.getDestinationUid(), _connection.getDestinationConnectable() });
		if (connectionWithNodeNumber != m_connectionNodeNumbers.end())
		{
			_connection.setNodeNumber(connectionWithNodeNumber->second);
			m_connectionNodeNumbers[{_connection.getOriginUid(), _connection.getOriginConnectable()}] = _connection.getNodeNumber();
		}
		else
		{
			connectionWithNodeNumber = m_connectionNodeNumbers.find({ _connection.getOriginUid(), _connection.getOriginConnectable() });
			if (connectionWithNodeNumber != m_connectionNodeNumbers.end())
			{
				_connection.setNodeNumber(connectionWithNodeNumber->second);
				m_connectionNodeNumbers[{_connection.getDestinationUid(), _connection.getDestinationConnectable()}] = _connection.getNodeNumber();
			}
			else
			{
				_connection.setNodeNumber(std::to_string(m_currentNodeNumber++));
				m_connectionNodeNumbers[{ _connection.getDestinationUid(), _connection.getDestinationConnectable() }] = _connection.getNodeNumber();
				m_connectionNodeNumbers[{ _connection.getOriginUid(), _connection.getOriginConnectable() }] = _connection.getNodeNumber();
			}
		}
	}
}

void NodeAssigner::setNodeNumbersOfVoltageSource(
	std::map<ot::UID, ot::UIDList>& _connectionBlockMap,
	std::string _startingElement, int _counter,
	ot::UID _startingElementUID, ot::UID _elementUID,
	std::map<ot::UID, std::shared_ptr<ot::EntityBlockConnection>> _allConnectionEntities,
	std::map<ot::UID, std::shared_ptr<ot::EntityBlock>>& _allEntitiesByBlockID,
	const std::string& _editorname, Circuit& _circuit,
	std::set<ot::UID>& _visitedElements)
{
	auto element = getEntityBlock(_allEntitiesByBlockID, _elementUID);
	if (element == nullptr)
	{
		return;
	}
	auto connections = getConnections(_connectionBlockMap, _elementUID);
	if (connections.empty())
	{
		return;
	}

	std::vector<Connection> connectionsToBeSet;
	for (auto connection : connections)
	{
		Connection myConn = createConnection(_allConnectionEntities, connection);
		connectionsToBeSet.push_back(myConn);
	}

	for (auto myConn = connectionsToBeSet.begin(); myConn != connectionsToBeSet.end(); ++myConn)
	{
		if (isGndVoltageSourceConnection(myConn->getOriginConnectable(), _startingElementUID, myConn->getOriginUid()) ||
			isGndVoltageSourceConnection(myConn->getDestinationConnectable(), _startingElementUID, myConn->getDestinationUid()))
		{
			myConn->setNodeNumber("0");
			m_connectionNodeNumbers[{ myConn->getDestinationUid(), myConn->getDestinationConnectable() }] = myConn->getNodeNumber();
			m_connectionNodeNumbers[{ myConn->getOriginUid(), myConn->getOriginConnectable() }] = myConn->getNodeNumber();

			_circuit.addConnection(myConn->getOriginConnectable(), myConn->getOriginUid(), *myConn);
			_circuit.addConnection(myConn->getDestinationConnectable(), myConn->getDestinationUid(), *myConn);
		}
	}
}

bool NodeAssigner::isVisited(std::set<ot::UID>& _visited, ot::UID _uid)
{
	if (_visited.find(_uid) != _visited.end())
	{
		return true;
	}
	else
	{
		_visited.insert(_uid);
		return false;
	}
}

Connection NodeAssigner::createConnection(std::map<ot::UID, std::shared_ptr<ot::EntityBlockConnection>> _allConnections, ot::UID _connectionUID)
{
	auto it = _allConnections.find(_connectionUID);
	if (it != _allConnections.end())
	{
		ot::GraphicsConnectionCfg connectionCfg = it->second->getConnectionCfg();
		Connection myConn(connectionCfg);
		return myConn;
	}
	else
	{
		OT_LOG_E("Connection does not exist - EntityID: " + _connectionUID);
		return Connection();
	}
}

bool NodeAssigner::isGNDConnection(const std::string& _pole) const
{
	return (_pole == m_gndPole);
}

bool NodeAssigner::isGndVoltageSourceConnection(const std::string& _pole, ot::UID _voltageSourceUID, ot::UID _elementUID) const
{
	return (_elementUID == _voltageSourceUID && _pole == "negativePole");
}

std::shared_ptr<ot::EntityBlock> NodeAssigner::getEntityBlock(std::map<ot::UID, std::shared_ptr<ot::EntityBlock>>& _allEntities, const ot::UID& _uid) const
{
	auto it = _allEntities.find(_uid);
	if (it != _allEntities.end())
	{
		return it->second;
	}
	OT_LOG_E("Block not found - EntityID: " + std::to_string(_uid));
	return nullptr;
}

ot::UIDList NodeAssigner::getConnections(std::map<ot::UID, ot::UIDList>& _connectionBlockMap, const ot::UID& _uid) const
{
	auto it = _connectionBlockMap.find(_uid);
	if (it != _connectionBlockMap.end())
	{
		return it->second;
	}
	OT_LOG_E("Connection not found - EntityID: " + std::to_string(_uid));
	return ot::UIDList{};
}