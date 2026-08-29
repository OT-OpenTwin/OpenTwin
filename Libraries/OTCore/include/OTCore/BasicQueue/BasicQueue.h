// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/CoreTypes.h"
#include "OTCore/RAII/FunctionRAII.h"

// std header
#include <mutex>
#include <queue>
#include <string>
#include <atomic>
#include <unordered_map>

namespace ot
{

	class BasicQueueObject;

	class OT_CORE_API_EXPORT BasicQueue
	{
		OT_DECL_NOCOPY(BasicQueue)
		OT_DECL_NOMOVE(BasicQueue)
	public:
		typedef std::string KeyType;

		BasicQueue();
		virtual ~BasicQueue();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Queue handling

		//! @brief Pushes a new object to the queue.
		//! The insert and unique handling must be fully defined in the object before pushing.
		//! @param _object The object to push to the queue. The queue takes ownership of the object.
		//! A unique object might get destroyed on insertion depending on the set unique insert type (e.g. ot::BasicQueueObject::UniqueQueueInsertType::SkipUnique is set and a unique object with the same key already exists).
		void push(BasicQueueObject* _object);

		//! @brief Executes the complete queue until it is empty or until stop() is called.
		void exec();

		void execNext();

		//! @brief Stops the queue execution after the current object has been executed.
		//! @param _clearQueue If true, the queue will be cleared and all remaining objects will be deleted. If false, the queue will be kept and can be resumed later.
		void stop(bool _clearQueue = false);

		bool hasNext();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Protected methods

	protected:
		virtual void exec(BasicQueueObject* _object);

		//! @brief Pops the next object from the queue and returns it. The caller is responsible for deleting the object after it has been executed.
		BasicQueueObject* pop();

	private:
		typedef std::list<BasicQueueObject*> QueueType;

		enum class State
		{
			None = 0 << 0,
			Executing = 1 << 0,
			StopRequested = 1 << 1
		};
		typedef Flags<State> StateFlags;

		void setState(State _state);
		void removeState(State _state);
		bool hasState(State _state);

		class StateRAII : public FunctionRAII
		{
			OT_DECL_NOCOPY(StateRAII)
			OT_DECL_NODEFAULT(StateRAII)
		public:
			StateRAII(BasicQueue& _queue, State _state)
				: FunctionRAII(nullptr, [&_queue, _state]() { _queue.removeState(_state); })
			{
				_queue.setState(_state);
			}

		};
		friend class StateRAII;

		std::mutex m_mutex;
		StateFlags m_state;
		QueueType m_queue;
		std::unordered_map<KeyType, QueueType::iterator> m_uniqueObjects;

	};

}