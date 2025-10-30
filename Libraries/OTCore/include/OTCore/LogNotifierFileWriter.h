// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/AbstractLogNotifier.h"

// std header
#include <mutex>

namespace ot {

	class OT_CORE_API_EXPORT LogNotifierFileWriter : public AbstractLogNotifier {
		OT_DECL_NODEFAULT(LogNotifierFileWriter)
		OT_DECL_NOCOPY(LogNotifierFileWriter)
	public:
		static std::string generateFileName(const std::string& _serviceName);

		LogNotifierFileWriter(const std::string& _filePath);
		virtual ~LogNotifierFileWriter();

		//! @brief Called when the a log message was created.
		virtual void log(const LogMessage& _message) override;

		void flushAndCloseStream(void);

		void closeStream(void);

	private:
		std::mutex m_mutex;
		std::ofstream* m_stream;
	};

}