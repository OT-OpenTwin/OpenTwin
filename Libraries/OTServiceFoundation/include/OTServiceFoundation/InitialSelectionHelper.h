// @otlicense

// OpenTwin header
#include "OTCore/CoreTypes.h"
#include "OTCommunication/Msg.h"
#include "OTServiceFoundation/FoundationAPIExport.h"

// std header
#include <mutex>
#include <string>

namespace ot {

	//! @brief The InitialSelectionHelper class helps to select an entity in the UI after a project is opened in the model.
	class OT_SERVICEFOUNDATION_API_EXPORT InitialSelectionHelper {
		OT_DECL_NOCOPY(InitialSelectionHelper)
		OT_DECL_NOMOVE(InitialSelectionHelper)
	public:
		enum Mode : int32_t {
			Undefined,
			SelectByName,
			SelectFirstChild
		};

		InitialSelectionHelper();
		~InitialSelectionHelper();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter

		//! @brief Set session open timeout.
		//! The default is 5 times ot::msg::defaultTimeout.
		void setSessionOpenTimeout(int64_t _timeout);

		void setMode(Mode _mode);
		void setEntityName(const std::string& _entityName);
		void setModelUrl(const std::string& _modelUrl);
		void setUiUrl(const std::string& _uiUrl);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Getter

		bool isReady();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Behavior

		//! @brief Runs the selection helper in a separate thread if all parameters are set.
		bool runIfReady();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Worker

	private:
		void reset();
		void worker();
		bool isTimeout(int64_t _initialTime);
		bool checkModelReady();
		void sendInitialSelectionToUI(const std::string& _entityToSelect);
		std::string determineEntityToSelect();

		std::string ensureEntityExistsByName(const std::string& _entityName);
		std::string determineFirstChildEntity(const std::string& _parentEntity);

		std::mutex m_mutex;

		int64_t m_sessionOpenTimeout;

		std::atomic_bool m_running;
		Mode m_mode;
		std::string m_uiUrl;
		std::string m_modelUrl;
		std::string m_entityName;

	};

}