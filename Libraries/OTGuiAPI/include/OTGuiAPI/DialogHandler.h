// @otlicense

// OpenTwin header
#include "OTGui/Dialog/PropertyDialogCfg.h"
#include "OTGuiAPI/OTGuiAPIAPIExport.h"

// std header
#include <string>
#include <optional>

namespace ot
{

	//! @class Frontend
	//! @brief Frontend request layer.
	//! The Frontend prvoides a set methods that may be used to send requests to the frontend.
	class OT_GUIAPI_API_EXPORT DialogHandler
	{
		OT_DECL_NOCOPY(DialogHandler)
		OT_DECL_NOMOVE(DialogHandler)
	public:
		DialogHandler() = default;

		static JsonDocument createDialogRequest(const PropertyDialogCfg& _cfg, const std::string& _callbackAction);

		static JsonDocument createDialogRequest(const DialogCfg* _cfg, const std::string& _requestAction, const std::string& _callbackAction, const std::optional<std::string>& _additionalInfo);
	};
}