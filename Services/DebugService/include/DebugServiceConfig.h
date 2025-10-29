// @otlicense

//! @file DebugServiceConfig.h
//! @author Alexander Kuester (alexk95)
//! @date March 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTSystem/Flags.h"
#include "OTCore/Serializable.h"
#include "OTCommunication/Msg.h"

//! @brief The DebugServiceConfig class is used to configure the debug service behaviour.
//! The configuration can be exported and imported as JSON.
class DebugServiceConfig : public ot::Serializable {
public:
	//! @brief The feature flags are used to configure the debug service behaviour.
	enum FeatureFlag {
		FeaturesDisabled  = 0 << 0, //! @brief No feature enabled (default).
		ExitOnCreation    = 1 << 0, //! @brief Exit the service on creation.
		ExitOnInit        = 1 << 0, //! @brief Exit the service on initialization action.
		ExitOnRun         = 1 << 1, //! @brief Exit the service on run action.
		ExitOnPing        = 1 << 2, //! @brief Exit the service on ping action.
		ExitOnPreShutdown = 1 << 3, //! @brief Exit the service on ping action.
		ExportOnStart     = 1 << 4  //! @brief Export the configuration on service start.
	};
	typedef ot::Flags<FeatureFlag> Features; //! @brief Feature flags type.

	static std::string toString(FeatureFlag _flag);
	static FeatureFlag stringToFeatureFlag(const std::string& _flag);

	static std::list<std::string> toStringList(const Features& _features);
	static Features stringListToFeatures(const std::list<std::string>& _features);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Constructor

	DebugServiceConfig() { this->reset(); };
	DebugServiceConfig(const Features& _features, int _sleepTime = (2 * ot::msg::defaultTimeout));
	virtual ~DebugServiceConfig() {};

	// ###########################################################################################################################################################################################################################################################################################################################

	// Virtual functions

	virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

	virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Setter / Getter

	inline void setFeatureFlag(FeatureFlag _flag, bool _set = true) { m_features.setFlag(_flag, _set); };
	inline void setFeatureFlags(const Features& _features) { m_features = _features; };
	inline const Features& getFeatureFlags(void) const { return m_features; };

	inline void reset(void) {
		m_features = FeatureFlag::FeaturesDisabled;
	}

private:
	Features m_features;
};

OT_ADD_FLAG_FUNCTIONS(DebugServiceConfig::FeatureFlag)