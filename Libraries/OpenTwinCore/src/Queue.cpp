//! @file Queue.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OpenTwinCore/Queue.h"
#include "OpenTwinCore/otAssert.h"
#include "OpenTwinCore/Logger.h"


#ifndef OT_CFG_QueueUseGarbageLimit
//! @brief If set to true the garbage collection will clear when reaching the QueueGarbageLimit
#define OT_CFG_QueueUseGarbageLimit false
#endif

#ifndef OT_CFG_QueueGarbageLimit
//! @brief The maximum ammount of garbage in the garbage collection (0 = max, 1 = no collection, 2+ = count)
//! Only has effect if OT_CFG_QueueUseGarbageLimit is set to true
#define OT_CFG_QueueGarbageLimit 0
#endif

ot::SimpleQueue::SimpleQueue() : m_garbageCounter(0) {

}

ot::SimpleQueue::~SimpleQueue() {
	clearGarbage();
}

void ot::SimpleQueue::queue(QueueObject* _obj, QueueData* _arg) noexcept {
	otAssert(_obj, "nullptr provided for queue");
	m_queue.push_back(std::pair<QueueObject*, QueueData*>(_obj, _arg));
}

void ot::SimpleQueue::queueNext(QueueObject* _obj, QueueData* _arg) noexcept {
	otAssert(_obj, "nullptr provided for queue");
	m_queue.push_front(std::pair<QueueObject*, QueueData*>(_obj, _arg));
}

bool ot::SimpleQueue::executeQueue(void) {
	std::pair<QueueObject*, QueueData*> itm = std::pair<QueueObject*, QueueData*>(nullptr, nullptr);
	while (!m_queue.empty()) {
		try {
			// Grab next queue item
			itm = m_queue.front();
			m_queue.pop_front();
			
			// Execute
			QueueObject::QueueResultFlags result = itm.first->activateFromQueue(this, itm.second);

			if (result.flagIsSet(QueueObject::JobFailed)) {
				OT_LOG_W("Job failed, ignoring");
			}

			// Check how to proceed after the call
			if (result.flagIsSet(QueueObject::CancelQueue)) {
				OT_LOG_D("Queue cancel result from queue object. Exiting queue");

				// memclear
				checkAndDestroy(itm.first);
				checkAndDestroy(itm.second);
				clearQueue();

				return false;
			}
			else if (result.flagIsSet(QueueObject::FinishQueue)) {
				OT_LOG_D("Queue finish result from queue object. Exiting queue");

				// memclear
				checkAndDestroy(itm.first);
				checkAndDestroy(itm.second);
				clearQueue();

				return true;
			}
			else if (result.flagIsSet(QueueObject::Requeue)) {
				m_queue.push_back(itm);
			}
			else {
				// memclear
				checkAndDestroy(itm.first);
				checkAndDestroy(itm.second);
			}
		}
		catch (const std::exception& _e) {
			otAssert(0, "Failed to execute queue: Exception caught");
			OT_LOG_E(std::string("Failed to execute queue: ") + _e.what());
			
			// memclear
			checkAndDestroy(itm.first);
			checkAndDestroy(itm.second);
			clearQueue();

			return false;
		}
		catch (...) {
			OT_LOG_EA("Failed to execute queue: Unknown error");
			clearQueue();
			return false;
		}
	}
	return true;
}

void ot::SimpleQueue::clearQueue(void) {
	for (auto obj : m_queue) {
		checkAndDestroy(obj.first);
		checkAndDestroy(obj.second);
	}
	m_queue.clear();
}

void ot::SimpleQueue::checkAndDestroy(QueueDestroyableObject* _obj) {
	// Check if the default cleanup should be avoided (creator keeps ownership)
	if (_obj->cleanupFlags() & QueueDestroyableObject::NoDelete) {
		return;
	}

	// Check if an instant delete is requested
	if (_obj->cleanupFlags() & QueueDestroyableObject::DeleteAfter) {
		delete _obj;
		return;
	}

	// Add object to garbage collection
	m_garbage.push_back(std::pair<QueueDestroyableObject*, bool>(_obj, (_obj->cleanupFlags() & QueueDestroyableObject::Multiuse) == QueueDestroyableObject::Multiuse));

#if (OT_CFG_QueueUseGarbageLimit == true)
	if (++m_garbageCounter == OT_CFG_QueueGarbageLimit) {
		clearGarbage();
	}
#endif
}

void ot::SimpleQueue::clearGarbage(void) {
	std::list<QueueDestroyableObject*> multiuse;
	for (auto o : m_garbage) {
		if (o.second) {
			if (std::find(multiuse.begin(), multiuse.end(), o.first) != multiuse.end()) continue;
			multiuse.push_back(o.first);
			delete o.first;
		}
	}
	m_garbage.clear();
	m_garbageCounter = 0;
}