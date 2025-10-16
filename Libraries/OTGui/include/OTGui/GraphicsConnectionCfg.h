//! @file GraphicsConnectionCfg.h
//! @author Alexander Kuester (alexk95)
//! @date October 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Color.h"
#include "OTCore/Point2D.h"
#include "OTCore/CoreTypes.h"
#include "OTCore/Serializable.h"
#include "OTGui/PenCfg.h"
#include "OTGui/OTGuiAPIExport.h"

// std header
#include <string>

#pragma warning(disable:4251)

namespace ot {

	class OT_GUI_API_EXPORT GraphicsConnectionCfg : public ot::Serializable {
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
		GraphicsConnectionCfg(const ot::UID& _originUid, const std::string& _originConnectableName, const ot::UID& _destinationUid, const std::string& _destinationName);
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

		//! @brief Create a copy of this connection but the origin an destination are swapped
		GraphicsConnectionCfg getReversedConnection() const;

		void setOriginUid(const ot::UID& _uid) { m_originUID = _uid; };
		const ot::UID& getOriginUid() const { return m_originUID; };

		void setOriginConnectable(const std::string& _name) { m_originConnectable = _name; };
		const std::string& getOriginConnectable() const { return m_originConnectable; };

		void setOriginPos(double _x, double _y) { m_originPos.set(_x, _y); };
		void setOriginPos(const ot::Point2DD& _pos) { m_originPos = _pos; };
		const ot::Point2DD& getOriginPos() const { return m_originPos; };

		void setDestUid(const ot::UID& _uid) { m_destUID = _uid; };
		const ot::UID& getDestinationUid() const { return m_destUID; };

		void setDestConnectable(const std::string& _name) { m_destConnectable = _name; };
		const std::string& getDestConnectable() const { return m_destConnectable; };

		void setDestPos(double _x, double _y) { m_destPos.set(_x, _y); };
		void setDestPos(const ot::Point2DD& _pos) { m_destPos = _pos; };
		const ot::Point2DD& getDestPos() const { return m_destPos; };

		void setUid(const ot::UID& _uid) { m_uid = _uid; };
		const ot::UID& getUid() const { return m_uid; };

		void setLineShape(ConnectionShape _shape) { m_lineShape = _shape; };
		ConnectionShape getLineShape() const { return m_lineShape; };

		void setLineWidth(double _width) { m_lineStyle.setWidth(_width); };
		double getLineWidth() const { return m_lineStyle.getWidth(); };

		void setLineColor(const ot::Color& _color) { m_lineStyle.setColor(_color); };

		//! @brief Set the line painter.
		//! The item takes ownership of the painter.
		void setLinePainter(ot::Painter2D* _painter) { m_lineStyle.setPainter(_painter); };

		//! @brief Returns the current painter.
		//! The item keeps ownership of the painter.
		const ot::Painter2D* getLinePainter() const { return m_lineStyle.getPainter(); };

		void setLineStyle(LineStyle _style) { m_lineStyle.setStyle(_style); };
		void setLineStyle(const PenFCfg& _style) { m_lineStyle = _style; };
		const PenFCfg& getLineStyle() const { return m_lineStyle; };

		std::string createConnectionKey() const;
		std::string createConnectionKeyReverse() const;

		void setHandlesState(bool _handleState) { m_handlesState = _handleState; };
		bool getHandlesState() const { return m_handlesState; };

	private:
		ot::UID m_originUID;
		std::string m_originConnectable;
		ot::Point2DD m_originPos;

		ot::UID m_destUID;
		std::string m_destConnectable;
		ot::Point2DD m_destPos;

		ot::UID m_uid;

		ConnectionShape m_lineShape;
		PenFCfg m_lineStyle;

		bool m_handlesState;
	};
}
