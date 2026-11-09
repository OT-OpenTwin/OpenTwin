// @otlicense
// File: QueueableObjectHandler.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

// OpenTwin header
#include "Component/QueueableObjectHandler.h"
#include "OTCore/BasicQueue/BasicQueueObject.h"
#include "OTCore/Logging/Logger.h"

QueueableObjectHandler::QueueableObjectHandler()
{}

QueueableObjectHandler::~QueueableObjectHandler()
{}

void QueueableObjectHandler::stopQueue(int _exitCode)
{
	m_queue.stop(_exitCode);
}

void QueueableObjectHandler::runOrQueue(ot::BasicQueueObject* _object)
{
	OTAssertNullptr(_object);
	std::unique_ptr<ot::BasicQueueObject> obj(_object);
	m_mutex.lock();
	if (this->isBlocked())
	{
		m_queue.push(obj.release());
		m_mutex.unlock();
	}
	else
	{
		m_mutex.unlock();
		obj->exec();
	}
}

ot::FunctionRAII QueueableObjectHandler::block(BlockReason _reason)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	auto it = m_blockReasons.find(_reason);
	if (it == m_blockReasons.end())
	{
		m_blockReasons.insert(std::make_pair(_reason, 1));
	}
	else
	{
		it->second++;
	}

	return ot::FunctionRAII(nullptr, [this, _reason]() { this->unblock(_reason); });
}

bool QueueableObjectHandler::isBlocked()
{
	int counter = 0;
	for (const auto& pair : m_blockReasons)
	{
		OTAssert(pair.second >= 0, "Invalid BlockReason counter state");
		counter += pair.second;
	}
	return counter > 0;
}

void QueueableObjectHandler::execQueue()
{
	m_queue.exec();
}

void QueueableObjectHandler::unblock(BlockReason _reason)
{
	m_mutex.lock();

	auto it = m_blockReasons.find(_reason);
	if (it != m_blockReasons.end())
	{
		OTAssert(it->second > 0, "Invalid BlockReason counter state");
		it->second--;
		if (it->second == 0)
		{
			m_blockReasons.erase(it);
		}
	}
	else
	{
		OT_LOG_E("Trying to unblock a reason that was not blocked.");
	}

	bool execRequired = !this->isBlocked();
	m_mutex.unlock();

	if (execRequired) {
		this->execQueue();
	}
}
