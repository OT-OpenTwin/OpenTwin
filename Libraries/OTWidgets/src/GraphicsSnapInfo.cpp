// @otlicense
// File: GraphicsSnapInfo.cpp
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

// OpenTwin header
#include "OTCore/Math.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/GraphicsItem.h"
#include "OTWidgets/GraphicsSnapInfo.h"
#include "OTWidgets/GraphicsConnectionItem.h"

void ot::GraphicsSnapInfo::addOriginSnap(const GraphicsItem* _item, const GraphicsItem* _connector, const GraphicsConnectionItem* _connection) {
	addSnap(_item, _connector, _connection, true);
}

void ot::GraphicsSnapInfo::addDestSnap(const GraphicsItem* _item, const GraphicsItem* _connector, const GraphicsConnectionItem* _connection) {
	addSnap(_item, _connector, _connection, false);
}

void ot::GraphicsSnapInfo::fillEvent(GraphicsChangeEvent& _event) const {
	for (const auto& snap : m_originSnaps) {
		GraphicsConnectionCfg cfg = snap.connection->getConfiguration();
		cfg.setOriginUid(snap.item->getGraphicsItemUid());
		cfg.setOriginConnectable(snap.connector->getGraphicsItemName());
		_event.addSnapInfo(cfg, true);
	}
	for (const auto& snap : m_destSnaps) {
		GraphicsConnectionCfg cfg = snap.connection->getConfiguration();
		cfg.setDestUid(snap.item->getGraphicsItemUid());
		cfg.setDestConnectable(snap.connector->getGraphicsItemName());
		_event.addSnapInfo(cfg, false);
	}
}

void ot::GraphicsSnapInfo::addSnap(const GraphicsItem* _item, const GraphicsItem* _connector, const GraphicsConnectionItem* _connection, bool _isOrigin) {
	OTAssertNullptr(_item);
	OTAssertNullptr(_connector);
	OTAssertNullptr(_connection);
	OTAssertNullptr(_connector->getQGraphicsItem());

	// Check for existing snap
	for (auto& snap : m_originSnaps) {
		if (snap.item == _item && snap.connector == _connector && snap.connection == _connection) {
			return;
		}
	}
	for (auto& snap : m_destSnaps) {
		if (snap.item == _item && snap.connector == _connector && snap.connection == _connection) {
			return;
		}
	}

	// Calculate distance
	const Point2DD connectorPos = QtFactory::toPoint2D(_connector->getQGraphicsItem()->mapToScene(_connector->getQGraphicsItem()->boundingRect().center()));
	Point2DD connectionPos;
	if (_isOrigin) {
		connectionPos = QtFactory::toPoint2D(_connection->getOriginPos());
	}
	else {
		connectionPos = QtFactory::toPoint2D(_connection->getDestPos());
	}
	
	double dist = Math::euclideanDistance(connectorPos, connectionPos);

	// Check for shorter snaps
	std::list<SnapInfo>& lst = _isOrigin ? m_originSnaps : m_destSnaps;

	for (auto it = lst.begin(); it != lst.end(); ) {
		if (it->connection == _connection) {
			if (dist >= it->distance) {
				// Shorter distance snap already exists
				return;
			}
			else {
				// Stored snap is longer, remove it
				it = lst.erase(it);
			}
		}
		else {
			it++;
		}
	}

	// Add snap
	lst.push_back(SnapInfo(_item, _connector, _connection, dist));
}
