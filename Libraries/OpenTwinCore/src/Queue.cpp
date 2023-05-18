//! @file Queue.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OpenTwinCore/Queue.h"
#include "OpenTwinCore/otAssert.h"
#include "OpenTwinCore/Logger.h"

ot::SimpleQueue::SimpleQueue() {

}

ot::SimpleQueue::~SimpleQueue() {
	clear();
}

void ot::SimpleQueue::queue(QueueObject* _obj, QueueData* _args) noexcept {
	otAssert(_obj, "nullptr provided for queue");
	m_queue.push_back(std::pair<QueueObject*, QueueData*>(_obj, _args));
}

void ot::SimpleQueue::queueNext(QueueObject* _obj, QueueData* _args) noexcept {
	otAssert(_obj, "nullptr provided for queue");
	m_queue.push_front(std::pair<QueueObject*, QueueData*>(_obj, _args));
}

bool ot::SimpleQueue::executeQueue(void) {
	while (!m_queue.empty()) {
		try {
			// Grab next queue item
			std::pair<QueueObject*, QueueData*> itm = m_queue.front();
			m_queue.pop_front();
			
			// Execute
			QueueObject::QueueResultFlags result = itm.first->activateFromQueue(this, itm.second);

			// Check if memory should be cleared
			bool memClr = true;
			if (result.flagIsSet(QueueObject::NoMemClear)) memClr = false;

			if (result.flagIsSet(QueueObject::CancelQueue)) {
				OT_LOG_WA("Queue cancel result from queue object. Cancelling queue");
				if (memClr) memClear(itm);
				clear();
				return false;
			}
			else if (result.flagIsSet(QueueObject::Requeue)) {
				memClr = false;
				m_queue.push_back(itm);
			}

			// Clear memory if needed
			if (memClr) memClear(itm);
		}
		catch (const std::exception& _e) {
			otAssert(0, "Failed to execute queue: Exception caught");
			OT_LOG_E(std::string("Failed to execute queue: ") + _e.what());
			clear();
			return false;
		}
		catch (...) {
			OT_LOG_EA("Failed to execute queue: Unknown error");
			clear();
			return false;
		}
	}
	return true;
}

void ot::SimpleQueue::clear(void) {
	for (auto obj : m_queue) memClear(obj);
	m_queue.clear();
}

void ot::SimpleQueue::memClear(const std::pair<QueueObject*, QueueData*>& _entry) {
	if (_entry.first) delete _entry.first;
	if (_entry.second) {
		// Delete data object if needed
		if (!_entry.second->isNoDeleteByQueue()) delete _entry.second;
	}
}