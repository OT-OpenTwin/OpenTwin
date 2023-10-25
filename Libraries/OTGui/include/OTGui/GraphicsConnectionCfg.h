//! @file GraphicsConnectionCfg.h
//! @author Alexander Kuester (alexk95)
//! @date October 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/OTGuiAPIExport.h"
#include "OpenTwinCore/Serializable.h"

// std header
#include <string>

#pragma warning(disable:4251)

namespace ot {

	class OT_GUI_API_EXPORT GraphicsConnectionCfg : public ot::Serializable {
	public:
		static std::string buildKey(const std::string& _originUid, const std::string& _originItemName, const std::string& _destUid, const std::string& _destItemName);

		GraphicsConnectionCfg();
		GraphicsConnectionCfg(const std::string& _originUid, const std::string& _originConnectableName, const std::string& _destinationUid, const std::string& _destinationName);
		GraphicsConnectionCfg(const GraphicsConnectionCfg& _other);
		virtual ~GraphicsConnectionCfg();

		GraphicsConnectionCfg& operator = (const GraphicsConnectionCfg& _other);

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(OT_rJSON_val& _object) override;

		//! @brief Creates a key that identifies this connection
		std::string buildKey(void) const;

		//! @brief Creates a key that identifies this connection, but the origin and destination are switched
		std::string buildReversedKey(void) const;

		void setOriginUid(const std::string& _uid) { m_originUID = _uid; };
		const std::string& originUid(void) const { return m_originUID; };

		void setOriginConnectable(const std::string& _name) { m_originConnectable = _name; };
		const std::string& originConnectable(void) const { return m_originConnectable; };

		void setDestUid(const std::string& _uid) { m_destUID = _uid; };
		const std::string& destUid(void) const { return m_destUID; };

		void setDestConnectable(const std::string& _name) { m_destConnectable = _name; };
		const std::string& destConnectable(void) const { return m_destConnectable; };

	private:
		std::string m_originUID;
		std::string m_originConnectable;
		std::string m_destUID;
		std::string m_destConnectable;
	};

}
