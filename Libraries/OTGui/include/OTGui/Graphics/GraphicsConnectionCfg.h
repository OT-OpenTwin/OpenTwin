// @otlicense
// File: GraphicsConnectionCfg.h
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

#pragma once

// OpenTwin header
#include "OTCore/Color.h"
#include "OTCore/Geometry/Point2D.h"
#include "OTGui/Style/PenCfg.h"
#include "OTGui/Graphics/GraphicsConnectionInfo.h"

// std header
#include <string>

#pragma warning(disable:4251)

namespace ot {

	class OT_GUI_API_EXPORT GraphicsConnectionCfg : public GraphicsConnectionInfo
	{
		OT_DECL_DEFCOPY(GraphicsConnectionCfg)
		OT_DECL_DEFMOVE(GraphicsConnectionCfg)
	public:
		enum class ConnectionShape {
			DirectLine,
			SmoothLine,
			XYLine,
			YXLine,
			AutoXYLine
		};

		static std::string shapeToString(ConnectionShape _shape);
		static ConnectionShape stringToShape(const std::string _shape);

		GraphicsConnectionCfg();
		GraphicsConnectionCfg(const ConstJsonObject& _jsonObject);
		GraphicsConnectionCfg(const UID& _originUid, const std::string& _originConnectableName, const UID& _destinationUid, const std::string& _destinationName);
		virtual ~GraphicsConnectionCfg();

		bool operator==(const GraphicsConnectionCfg& _other) const;

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		//! @brief Create a copy of this connection configuration with reversed origin and destination.
		GraphicsConnectionCfg getReversedConnectionCfg() const;

		void setOriginPos(double _x, double _y) { m_originPos.set(_x, _y); };
		void setOriginPos(const Point2DD& _pos) { m_originPos = _pos; };
		const Point2DD& getOriginPos() const { return m_originPos; };

		void setDestinationPos(double _x, double _y) { m_destPos.set(_x, _y); };
		void setDestinationPos(const Point2DD& _pos) { m_destPos = _pos; };
		const Point2DD& getDestinationPos() const { return m_destPos; };

		void setUid(const UID& _uid) { m_uid = _uid; };
		const UID& getUid() const { return m_uid; };

		void setLineShape(ConnectionShape _shape) { m_lineShape = _shape; };
		ConnectionShape getLineShape() const { return m_lineShape; };

		void setLineWidth(double _width) { m_lineStyle.setWidth(_width); };
		double getLineWidth() const { return m_lineStyle.getWidth(); };

		void setLineColor(const Color& _color) { m_lineStyle.setColor(_color); };

		//! @brief Set the line painter.
		//! The item takes ownership of the painter.
		void setLinePainter(Painter2D* _painter) { m_lineStyle.setPainter(_painter); };

		//! @brief Returns the current painter.
		//! The item keeps ownership of the painter.
		const Painter2D* getLinePainter() const { return m_lineStyle.getPainter(); };

		void setLineStyle(LineStyle _style) { m_lineStyle.setStyle(_style); };
		void setLineStyle(const PenFCfg& _style) { m_lineStyle = _style; };
		const PenFCfg& getLineStyle() const { return m_lineStyle; };

		void setHandlesState(bool _handleState) { m_handlesState = _handleState; };
		bool getHandlesState() const { return m_handlesState; };

	private:
		Point2DD m_originPos;
		Point2DD m_destPos;

		UID m_uid;

		ConnectionShape m_lineShape;
		PenFCfg m_lineStyle;

		bool m_handlesState;
	};
}
