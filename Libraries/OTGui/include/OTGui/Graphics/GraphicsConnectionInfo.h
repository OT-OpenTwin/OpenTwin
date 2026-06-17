// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/Serializable.h"
#include "OTGui/GuiTypes.h"

namespace ot
{

	class OT_GUI_API_EXPORT GraphicsConnectionInfo : public Serializable
	{
		OT_DECL_DEFCOPY(GraphicsConnectionInfo)
		OT_DECL_DEFMOVE(GraphicsConnectionInfo)
	public:
		GraphicsConnectionInfo() = default;
		GraphicsConnectionInfo(UID _originUid, const std::string& _originConnectableName, UID _destinationUid, const std::string& _destinationConnectableName);
		GraphicsConnectionInfo(const ConstJsonObject& _jsonObject);
		virtual ~GraphicsConnectionInfo() = default;

		virtual void addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _jsonObject) override;

		void setUid(const UID& _uid) { m_uid = _uid; };
		const UID& getUid() const { return m_uid; };

		//! @brief Returns true if the origin UID is valid.
		bool hasOrigin() const { return m_originUID != ot::invalidUID; };

		void setOriginUid(const ot::UID& _uid) { m_originUID = _uid; };
		const ot::UID& getOriginUid() const { return m_originUID; };

		void setOriginConnectable(const std::string& _name) { m_originConnectable = _name; };
		const std::string& getOriginConnectable() const { return m_originConnectable; };

		//! @brief Returns true if the destination UID is valid.
		bool hasDestination() const { return m_destUID != ot::invalidUID; };

		void setDestinationUid(const ot::UID& _uid) { m_destUID = _uid; };
		const ot::UID& getDestinationUid() const { return m_destUID; };

		void setDestinationConnectable(const std::string& _name) { m_destConnectable = _name; };
		const std::string& getDestinationConnectable() const { return m_destConnectable; };

		//! @brief Create a copy of this connection information with reversed origin and destination.
		GraphicsConnectionInfo getReversedConnectionInfo() const;

		std::string createConnectionKey() const;
		std::string createConnectionKeyReverse() const;

	private:
		UID m_uid = ot::invalidUID;

		ot::UID m_originUID = ot::invalidUID;
		std::string m_originConnectable;

		ot::UID m_destUID = ot::invalidUID;
		std::string m_destConnectable;

	};

}