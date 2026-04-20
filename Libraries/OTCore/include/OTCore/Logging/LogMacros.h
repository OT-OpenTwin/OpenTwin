// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/Logging/LogDispatcher.h"

#if !defined(OT_AssertOnLog)
#define OT_AssertOnLog false
#endif

#if OT_AssertOnLog==true
#define OT_LOG_ASSERT(___message) OTAssert(0, ___message)
#else
#define OT_LOG_ASSERT(___message)
#endif


#ifdef _DEBUG

//! @brief Log a message according to the service logger configuration and the provided flags.
//! @param ___text The log message text.
//! @param ___flags LogFlags describing the type of the created log message.
#define OT_LOG(___text, ___flags) ot::LogDispatcher::instance().dispatch(___text, __FUNCTION__, ___flags)

//! @brief Log a message according to the service logger configuration and the provided flags using the LogMessageStream.
//! @param ___streamPipeline The log message stream pipeline (e.g. "Value: " << value).
//! @param ___flags LogFlags describing the type of the created log message.
#define OT_LOG_STREAM(___streamPipeline, ___flags) { ot::LogMessageStream ot_intern_LogMessageStream(__FUNCTION__, ___flags); ot_intern_LogMessageStream << ___streamPipeline; ot::LogDispatcher::instance().dispatch(ot_intern_LogMessageStream); }

#else

//! @brief Log a message according to the service logger configuration and the provided flags.
//! @param ___text The log message text.
//! @param ___flags LogFlags describing the type of the created log message.
#define OT_LOG(___text, ___flags) if (ot::LogDispatcher::mayLog(___flags)) { ot::LogDispatcher::instance().dispatch(___text, __FUNCTION__, ___flags); }

//! @brief Log a message according to the service logger configuration and the provided flags using the LogMessageStream.
//! @param ___streamPipeline The log message stream pipeline (e.g. "Value: " << value).
//! @param ___flags LogFlags describing the type of the created log message.
#define OT_LOG_STREAM(___streamPipeline, ___flags) if (ot::LogDispatcher::mayLog(___flags)) { ot::LogMessageStream ot_intern_LogMessageStream(__FUNCTION__, ___flags); ot_intern_LogMessageStream << ___streamPipeline; ot::LogDispatcher::instance().dispatch(ot_intern_LogMessageStream); }

#endif

//! @brief Log a information message according to the logger configuration.
//! Information messages should contain general information.
//! @param ___text The log message.
#define OT_LOG_I(___text) OT_LOG(___text, ot::INFORMATION_LOG)

//! @brief Log a information message according to the logger configuration using the LogMessageStream.
//! Information messages should contain general information.
//! @param ___streamPipeline The log message stream pipeline (e.g. "Value: " << value).
#define OT_LOG_IS(___streamPipeline) OT_LOG_STREAM(___streamPipeline, ot::INFORMATION_LOG)

//! @brief Log a detailed information message according to the logger configuration.
//! Detailed information messages may contain more information than regular information messages.
//! @param ___text The log message.
#define OT_LOG_D(___text) OT_LOG(___text, ot::DETAILED_LOG)

//! @brief Log a detailed information message according to the logger configuration using the LogMessageStream.
//! Detailed information messages may contain more information than regular information messages.
//! @param ___streamPipeline The log message stream pipeline (e.g. "Value: " << value).
#define OT_LOG_DS(___streamPipeline) OT_LOG_STREAM(___streamPipeline, ot::DETAILED_LOG)

//! @brief Log a warning message according to the logger configuration.
//! @param ___text The log message.
#define OT_LOG_W(___text) OT_LOG_ASSERT("[OpenTwin] [Warning]"); OT_LOG(___text, ot::WARNING_LOG)

//! @brief Log a warning message according to the logger configuration using the LogMessageStream.
//! @param ___streamPipeline The log message stream pipeline (e.g. "Value: " << value).
#define OT_LOG_WS(___streamPipeline) OT_LOG_ASSERT("[OpenTwin] [Warning]"); OT_LOG_STREAM(___streamPipeline, ot::WARNING_LOG)

//! @brief Log a error message according to the logger configuration.
//! @param ___text The log message.
#define OT_LOG_E(___text) OT_LOG_ASSERT("[OpenTwin] [Error]"); OT_LOG(___text, ot::ERROR_LOG)

//! @brief Log a error message according to the logger configuration using the LogMessageStream.
//! @param ___streamPipeline The log message stream pipeline (e.g. "Value: " << value).
#define OT_LOG_ES(___streamPipeline) OT_LOG_ASSERT("[OpenTwin] [Error]"); OT_LOG_STREAM(___streamPipeline, ot::ERROR_LOG)

//! @brief Log a test message according to the logger configuration.
//! @param ___text The log message.
#define OT_LOG_T(___text) OT_LOG(___text, ot::TEST_LOG)

//! @brief Log a test message according to the logger configuration using the LogMessageStream.
#define OT_LOG_TS(___streamPipeline) OT_LOG_STREAM(___streamPipeline, ot::TEST_LOG)

//! @brief Log a memory pointer address along with a message.
//! The memory address will be logged as 16 digit hexadecimal value with leading zeros and "0x" prefix.
//! @param ___ptr Pointer to log.
//! @param ___message Message to log.
#define OT_LOG_MEM(___ptr, ___message) OT_LOG_T(___message + std::string(" 0x") + ot::String::ptrToHexString(___ptr))

// ###########################################################################################################################################################################################################################################################################################################################

// User log macros

//! @brief Log a user log message.
//! @param ___text The log message text.
//! @param ___flags LogFlags describing the type of the created log message.
#define OT_USER_LOG(___text, ___flags) ot::LogDispatcher::instance().dispatchUserLog(___text, __FUNCTION__, ___flags)

//! @brief Log a user log message using the LogMessageStream.
//! @param ___streamPipeline The log message stream pipeline (e.g. "Value: " << value).
//! @param ___flags LogFlags describing the type of the created log message.
#define OT_USER_LOG_STREAM(___streamPipeline, ___flags) { ot::LogMessageStream ot_intern_LogMessageStream(__FUNCTION__, ___flags); ot_intern_LogMessageStream << ___streamPipeline; ot::LogDispatcher::instance().dispatchUserLog(ot_intern_LogMessageStream); }

//! @brief Log a information user message.
//! @param ___text The log message text.
#define OT_USER_LOG_I(___text) OT_USER_LOG(___text, ot::INFORMATION_LOG)

//! @brief Log a information user message using the LogMessageStream.
//! @param ___streamPipeline The log message stream pipeline (e.g. "Value: " << value).
#define OT_USER_LOG_IS(___streamPipeline) OT_USER_LOG_STREAM(___streamPipeline, ot::INFORMATION_LOG)

//! @brief Log a detailed information user message.
//! @param ___text The log message text.
#define OT_USER_LOG_D(___text) OT_USER_LOG(___text, ot::DETAILED_LOG)

//! @brief Log a detailed information user message using the LogMessageStream.
//! @param ___streamPipeline The log message stream pipeline (e.g. "Value: " << value).
#define OT_USER_LOG_DS(___streamPipeline) OT_USER_LOG_STREAM(___streamPipeline, ot::DETAILED_LOG)

//! @brief Log a warning user message.
//! @param ___text The log message text.
#define OT_USER_LOG_W(___text) OT_USER_LOG(___text, ot::WARNING_LOG)

//! @brief Log a warning user message using the LogMessageStream.
//! @param ___streamPipeline The log message stream pipeline (e.g. "Value: " << value).
#define OT_USER_LOG_WS(___streamPipeline) OT_USER_LOG_STREAM(___streamPipeline, ot::WARNING_LOG)

//! @brief Log a error user message.
//! @param ___text The log message text.
#define OT_USER_LOG_E(___text) OT_USER_LOG(___text, ot::ERROR_LOG)

//! @brief Log a error user message using the LogMessageStream.
//! @param ___streamPipeline The log message stream pipeline (e.g. "Value: " << value).
#define OT_USER_LOG_ES(___streamPipeline) OT_USER_LOG_STREAM(___streamPipeline, ot::ERROR_LOG)

//! @brief Log a test user message.
//! @param ___text The log message text.
#define OT_USER_LOG_T(___text) OT_USER_LOG(___text, ot::TEST_LOG)

//! @brief Log a test user message using the LogMessageStream.
//! @param ___streamPipeline The log message stream pipeline (e.g. "Value: " << value).
#define OT_USER_LOG_TS(___streamPipeline) OT_USER_LOG_STREAM(___streamPipeline, ot::TEST_LOG)
