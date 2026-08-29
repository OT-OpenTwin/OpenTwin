// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/BasicQueue/BasicQueue.h"
#include "OTCore/BasicQueue/BasicQueueObject.h"

ot::BasicQueue::BasicQueue()
//	: m_isExecuting(false), m_isStopped(false)
{}

ot::BasicQueue::~BasicQueue()
{
	
}

// ###########################################################################################################################################################################################################################################################################################################################

// Queue handling

void ot::BasicQueue::push(BasicQueueObject * _object)
{

}

void ot::BasicQueue::exec()
{
	
}

void ot::BasicQueue::execNext()
{
	BasicQueueObject* object = pop();
	if (object) {
		exec(object);
		delete object;
	}
}

void ot::BasicQueue::stop(bool _clearQueue)
{
	
	
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

// Protected methods

void ot::BasicQueue::exec(BasicQueueObject * _object)
{

}

ot::BasicQueueObject * ot::BasicQueue::pop()
{
	ot::BasicQueueObject* object = nullptr;
	m_mutex.lock();
	if (!m_queue.empty()) {
		object = m_queue.front();
		m_queue.pop_front();
	}
	m_mutex.unlock();
	return object;
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
