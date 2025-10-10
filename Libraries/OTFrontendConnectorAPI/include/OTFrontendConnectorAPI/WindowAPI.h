//! @file WindowAPI.h
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTFrontendConnectorAPI/OTFrontendConnectorAPIExport.h"

// Qt header
#include <QtCore/qstring.h>

namespace ot {

	class OT_FRONTEND_CONNECTOR_API_EXPORT WindowAPI {
	public:
		static void lockUI(bool _flag);
		static void lockSelectionAndModification(bool _flag);

		//! @brief Will turn on or off the progress bar visibility and set the progress message
		static void setProgressBarVisibility(QString _progressMessage, bool _progressBaseVisible, bool _continuous);

		//! @brief Will set the percentage of the progress bar
		static void setProgressBarValue(int _progressPercentage);

	protected:
		virtual void lockUIAPI(bool _flag) = 0;
		virtual void lockSelectionAndModificationAPI(bool _flag) = 0;

		virtual void setProgressBarVisibilityAPI(QString _progressMessage, bool _progressBaseVisible, bool _continuous) = 0;
		virtual void setProgressBarValueAPI(int _progressPercentage) = 0;

		WindowAPI();
		virtual ~WindowAPI();

	private:
		static WindowAPI* instance();
		static WindowAPI*& getInstanceReference();

	};

}