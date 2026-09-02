// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/RAII/FunctionRAII.h"
#include "OTCore/BasicQueue/BasicQueue.h"

// std header
#include <map>
#include <stack>
#include <mutex>

class QueueableObjectHandler
{
	OT_DECL_NOCOPY(QueueableObjectHandler)
	OT_DECL_NOMOVE(QueueableObjectHandler)
public:
	enum BlockReason : int32_t
	{
		CompoundMessage,
		MulticloseViews,
		//! @brief Minimum value from custom block reasons.
		//! The custom block reason
		Custom = 10000
	};

	QueueableObjectHandler();
	virtual ~QueueableObjectHandler();

	void stopQueue(int _exitCode = ot::AppExitCode::GeneralExitCode::GeneralError);

	//! @brief Executes the given object immediately if the queue is not blocked, otherwise pushes it to the queue.
	void runOrQueue(ot::BasicQueueObject* _object);

	//! @brief Blocks the immediate execution of objects and pushes them to the queue instead.
	//! This method is not thread-safe and should only be called from the "main" thread.
	OT_DECL_NODISCARD ot::FunctionRAII block(BlockReason _reason);

private:
	//! @note The mutex must be locked before calling this method.
	bool isBlocked();
	void execQueue();
	void unblock(BlockReason _reason);

	std::mutex m_mutex;
	std::map<BlockReason, int> m_blockReasons;
	ot::BasicQueue m_queue;
};

