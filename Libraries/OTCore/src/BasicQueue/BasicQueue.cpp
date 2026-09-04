// @otlicense

#pragma once

// OpenTwin header
#include "OTSystem/Exception.h"
#include "OTCore/Logging/Logger.h"
#include "OTCore/RAII/ValueRAII.h"
#include "OTCore/BasicQueue/BasicQueue.h"
#include "OTCore/BasicQueue/BasicQueueObject.h"

ot::BasicQueue::BasicQueue()
	: m_state(State::None), m_currentObject(nullptr), m_exitCode(AppExitCode::Success)
{}

ot::BasicQueue::~BasicQueue()
{

}

// ###########################################################################################################################################################################################################################################################################################################################

// Queue handling

void ot::BasicQueue::push(BasicQueueObject* _object)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	std::string newKey = _object->getQueueObjectKey();

	// Handle unique objects
	if (_object->isQueueObjectUnique())
	{
		auto it = m_uniqueObjects.find(newKey);
		if (it != m_uniqueObjects.end())
		{
			BasicQueueObject* existingObject = *(it->second);
			OTAssertNullptr(existingObject);

			switch (_object->getUniqueInsertBehavior())
			{
			case BasicQueueObject::KeepExisting:
				// Unique object already exists, so we ignore the new one and delete it
				delete _object;
				return;

			case BasicQueueObject::ReplaceExisting:
				// Unique object already exists, so we replace it with the new one
				*(it->second) = _object;
				delete existingObject;
				return;

			case BasicQueueObject::RemoveExisting:
				// Unique object already exists, so we remove it from the queue and insert the new one according to its insert order
				m_queue.erase(it->second);
				m_uniqueObjects.erase(it);
				delete existingObject;
				break;
			default:
				break;
			}
		}
	}

	switch (_object->getInsertOrder())
	{
	case BasicQueueObject::InsertOrder::InsertBack:
		m_queue.push_back(_object);
		if (_object->isQueueObjectUnique())
		{
			m_uniqueObjects.insert_or_assign(newKey, std::prev(m_queue.end()));
		}
		break;

	case BasicQueueObject::InsertOrder::InsertFront:
		m_queue.push_front(_object);
		if (_object->isQueueObjectUnique())
		{
			m_uniqueObjects.insert_or_assign(newKey, m_queue.begin());
		}
		break;

	default:
		OT_LOG_ES("Unknown insert order for BasicQueueObject { \"Key\"" << newKey << "\" }");
		break;
	}
}

int ot::BasicQueue::exec()
{
	// Ensure that the queue is not already running
	m_mutex.lock();           // LOCK
	if (m_state.hasAny(State::ExectutingStates))
	{
		m_mutex.unlock();
		//OT_LOG_E("Queue is already running.");
		return AppExitCode::QueueExecutionBlocked;
	}

	if (m_queue.empty())
	{
		m_mutex.unlock();     // UNLOCK
		return AppExitCode::Success;
	}

	// Create the reset raii which will remove the state
	StateRAII stateRAII(*this, State::ExecutingAll);
	std::unique_ptr<BasicQueueObject> object(m_queue.front());
	m_queue.pop_front();
	m_mutex.unlock();         // UNLOCK

	// Grab the first object from the queue

	while (true)
	{
		m_mutex.lock();       // LOCK

		// Set the current object for the duration of its execution
		m_currentObject = object.get();
		OTAssertNullptr(m_currentObject);
		removeIfUnique(m_currentObject);
		
		m_mutex.unlock();     // UNLOCK

		int exitCode = this->execObject(object.get());
		
		m_mutex.lock();       // LOCK

		// Reset the current object after execution
		m_currentObject = nullptr;

		// Destroy the object after execution
		object.reset();

		// Check if the queue should stop or if there are no more objects to execute
		if (m_state.has(State::StopRequested))
		{
			m_mutex.unlock(); // UNLOCK
			return m_exitCode;
		}
		if (exitCode < AppExitCode::StateExitCode_IteratorFirst || exitCode > AppExitCode::StateExitCode_IteratorLast)
		{
			m_mutex.unlock(); // UNLOCK
			return exitCode;
		}
		if (m_queue.empty())
		{
			m_mutex.unlock(); // UNLOCK
			break;
		}

		// Grab the next object from the queue
		object.reset(m_queue.front());
		m_queue.pop_front();
		m_mutex.unlock();     // UNLOCK
	}

	return AppExitCode::Success;
}

void ot::BasicQueue::stop(int _exitCode)
{
	m_mutex.lock();
	if (!m_state.hasAny(State::ExectutingStates))
	{
		m_mutex.unlock();
		return;
	}

	m_exitCode = _exitCode;

	StateRAII stateRAII(*this, State::StopRequested);
	m_mutex.unlock();

	while (this->isRunning())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

bool ot::BasicQueue::hasNext()
{
	bool result = false;
	m_mutex.lock();
	result = !m_queue.empty();
	m_mutex.unlock();
	return result;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Protected methods: Execution

int ot::BasicQueue::execObject(BasicQueueObject* _object)
{
	return _object->exec();
}

ot::BasicQueueObject* ot::BasicQueue::pop()
{
	ot::BasicQueueObject* object = nullptr;
	m_mutex.lock();
	if (!m_queue.empty())
	{
		object = m_queue.front();
		m_queue.pop_front();
	}
	m_mutex.unlock();
	return object;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Protected methods: State

void ot::BasicQueue::removeIfUnique(BasicQueueObject* _object)
{
	if (_object && _object->isQueueObjectUnique())
	{
		std::string key = _object->getQueueObjectKey();
		m_uniqueObjects.erase(key);
	}
}

void ot::BasicQueue::setState(State _state)
{
	m_mutex.lock();
	m_state.set(_state);
	m_mutex.unlock();
}

void ot::BasicQueue::removeState(State _state)
{
	m_mutex.lock();
	m_state.remove(_state);
	m_mutex.unlock();
}

bool ot::BasicQueue::hasState(State _state)
{
	bool result = false;
	m_mutex.lock();
	result = m_state.has(_state);
	m_mutex.unlock();
	return result;
}

bool ot::BasicQueue::hasAnyState(const StateFlags& _state)
{
	bool result = false;
	m_mutex.lock();
	result = m_state.hasAny(_state);
	m_mutex.unlock();
	return result;
}
