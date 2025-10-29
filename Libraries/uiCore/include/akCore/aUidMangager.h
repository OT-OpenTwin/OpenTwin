// @otlicense

#pragma once

// C++ header
#include <mutex>						// mutex

// AK header
#include <akCore/globalDataTypes.h>		// API Export

namespace ak {

	//! This class is used to manage UIDs.
	//! This UID manager is incrementing the UID on each getId call.
	class UICORE_API_EXPORT aUidManager {
	public:

		//! @brief Default construcor. Initialize the first id with 1
		aUidManager();

		//! @brief Deconstructor
		virtual ~aUidManager();

		// ############################################################################

		//! @brief Will return the current id and increase it for the next call
		virtual UID getId(void);

		//! @brief Will reset the UID manager (start from the beginning)
		void reset(void);

		//! @brief Will set the current UID to the UID provided
		//! @param _uid The new current _uid
		void setLatestUid(UID _uid);

	private:
		UID				m_currentId;						//! Contains the current id
		std::mutex		m_mutex;							//! Used for thread protection

		// Block copy constructor
		aUidManager(const aUidManager & other) = delete;

		// Block assignment operator
		aUidManager & operator = (const aUidManager & other) = delete;

	};
}