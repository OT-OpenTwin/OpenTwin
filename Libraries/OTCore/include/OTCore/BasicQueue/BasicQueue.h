// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/CoreTypes.h"
#include "OTCore/RAII/FunctionRAII.h"
#include "OTCore/Object/ObjectDestroyNotifier.h"

// std header
#include <mutex>
#include <queue>
#include <string>
#include <atomic>
#include <unordered_map>

namespace ot
{

	class BasicQueueObject;

	//! @brief The BasicQueue class provides a thread-safe queue for executing BasicQueueObject instances in a sequential manner.
	//! The queue can be executed in a single thread, and it supports unique objects that can only exist once in the queue at a time.
	class OT_CORE_API_EXPORT BasicQueue
	{
		OT_DECL_NOCOPY(BasicQueue)
		OT_DECL_NOMOVE(BasicQueue)
	protected:
		enum class State
		{
			None = 0 << 0,
			ExecutingAll = 1 << 0,
			ExecutingNext = 1 << 1,
			StopRequested = 1 << 2,

			ExectutingStates = ExecutingAll | ExecutingNext
		};
		typedef Flags<State> StateFlags;
		OT_ADD_FRIEND_FLAG_FUNCTIONS(State, StateFlags)

	public:
		typedef std::string KeyType;

		explicit BasicQueue();
		virtual ~BasicQueue();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Queue handling

		//! @brief Pushes a new object to the queue.
		//! The insert and unique handling must be fully defined in the object before pushing.
		//! @param _object The object to push to the queue. The queue takes ownership of the object.
		//! A unique object might get destroyed on insertion depending on the set unique insert type (e.g. ot::BasicQueueObject::UniqueQueueInsertType::SkipUnique is set and a unique object with the same key already exists).
		void push(BasicQueueObject* _object);

		//! @brief Executes the complete queue until it is empty, until one of the objects returned a non zero exit code or until stop() is called.
		//! @return The exit code of the last executed object or the exit code set by stop().
		//! @throw ot::RuntimeException if the queue is already running.
		int exec();

		//! @brief Stops the queue execution after the current object has been executed.
		//! @param _exitCode The exit code to return from exec(). The default is ot::AppExitCode::GeneralExitCode::GeneralError.
		void stop(int _exitCode = AppExitCode::GeneralError);

		OT_DECL_NODISCARD bool hasNext();

		//! @brief Returns true if the queue is currently executing.
		//! This method is thread-safe and can be called from any thread.
		//! This method will lock the mutex during the check.
		OT_DECL_NODISCARD bool isRunning() { return this->hasAnyState(State::ExecutingAll | State::ExecutingNext); };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Protected methods: Execution

	protected:
		//! @brief Executes the given object.
		//! This method can be overridden by inheriting classes to provide custom execution behavior.
		//! The mutex state is unlocked when this method is called.
		//! The default implementation calls the exec() method of the object.
		OT_DECL_NODISCARD virtual int execObject(BasicQueueObject* _object);

		//! @brief Pops the next object from the queue and returns it. The caller is responsible for deleting the object after it has been executed.
		OT_DECL_NODISCARD BasicQueueObject* pop();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Protected methods: State

		//! @brief Returns the current state of the queue.
		//! @note This method is not thread-safe and should only be called from the thread that currently locks the mutex.
		//! Use hasState() or hasAnyState() to check the state thread-safe.
		OT_DECL_NODISCARD const StateFlags& getState() const { return m_state; };

		//! @brief Returns true if the queue has the given state.
		//! This method is thread-safe and can be called from any thread.
		//! This method will lock the mutex during the check.
		OT_DECL_NODISCARD bool hasState(State _state);

		//! @brief Returns true if the queue has any of the given states.
		//! This method is thread-safe and can be called from any thread.
		//! This method will lock the mutex during the check.
		OT_DECL_NODISCARD bool hasAnyState(const StateFlags& _state);

	private:
		friend class BasicQueueObject;
		typedef std::list<BasicQueueObject*> QueueType;

		//! @brief Sets the provided stte flag.
		void setState(State _state);

		//! @brief Removes the provided state flag.
		void removeState(State _state);		

		class StateRAII : public FunctionRAII
		{
			OT_DECL_NOCOPY(StateRAII)
			OT_DECL_NODEFAULT(StateRAII)
		public:

			//! @brief Create the raii object.
			//! The mutex must be locked on creation.
			StateRAII(BasicQueue& _queue, State _state)
				: FunctionRAII(nullptr, [&_queue, _state]() { _queue.removeState(_state); })
			{
				_queue.m_state.set(_state);
			}

		};
		friend class StateRAII;

		std::mutex m_mutex;
		StateFlags m_state;
		int m_exitCode;
		QueueType m_queue;
		std::unordered_map<KeyType, QueueType::iterator> m_uniqueObjects;
		BasicQueueObject* m_currentObject;
	};

}