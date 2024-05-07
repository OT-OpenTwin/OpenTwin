//! @file GraphicsConnectionCfg.h
//! @author Alexander Kuester (alexk95)
//! @date October 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/OTGuiAPIExport.h"
#include "OTCore/Color.h"
#include "OTCore/Serializable.h"
#include "OTCore/CoreTypes.h"
// std header
#include <string>

#pragma warning(disable:4251)

namespace ot {

	class OT_GUI_API_EXPORT GraphicsConnectionCfg : public ot::Serializable {
	public:
		enum ConnectionStyle {
			DirectLine,
			SmoothLine
		};

		static std::string styleToString(ConnectionStyle _style);
		static ConnectionStyle stringToStyle(const std::string _style);

		GraphicsConnectionCfg();
		GraphicsConnectionCfg(const ot::UID& _originUid, const std::string& _originConnectableName, const ot::UID& _destinationUid, const std::string& _destinationName);
		GraphicsConnectionCfg(const GraphicsConnectionCfg& _other);
		virtual ~GraphicsConnectionCfg();

		GraphicsConnectionCfg& operator = (const GraphicsConnectionCfg& _other);
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
		GraphicsConnectionCfg getReversedConnection(void) const;

		void setOriginUid(const ot::UID& _uid) { m_originUID = _uid; };
		const ot::UID& getOriginUid(void) const { return m_originUID; };

		void setOriginConnectable(const std::string& _name) { m_originConnectable = _name; };
		const std::string& originConnectable(void) const { return m_originConnectable; };

		void setDestUid(const ot::UID& _uid) { m_destUID = _uid; };
		const ot::UID& getDestinationUid(void) const { return m_destUID; };

		void setDestConnectable(const std::string& _name) { m_destConnectable = _name; };
		const std::string& destConnectable(void) const { return m_destConnectable; };

		void setUid(const ot::UID& _uid) { m_uid = _uid; };
		const ot::UID& getUid(void) const { return m_uid; };

		void setLineWidth(int _width) { m_width = _width; };
		int lineWidth(void) const { return m_width; };

		void setColor(const ot::Color& _color) { m_color = _color; };
		const ot::Color& color(void) const { return m_color; };

		void setStyle(ConnectionStyle _style) { m_style = _style; };
		ConnectionStyle style(void) const { return m_style; };

		std::string createConnectionKey() const;
		std::string createConnectionKeyReverse() const;

	private:
		ot::UID m_originUID;
		std::string m_originConnectable;

		ot::UID m_destUID;
		std::string m_destConnectable;

		ot::UID m_uid;

		int m_width;
		ot::Color m_color;

		ConnectionStyle m_style;
	};
}
