//! @file Queue.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OpenTwinCore/CoreAPIExport.h"
#include "OpenTwinCore/Flags.h"

// std header
#include <list>

#pragma warning (disable:4251)

namespace ot {

	class __declspec(dllexport) QueueData {
	public:
		QueueData() {};
		virtual ~QueueData() {};

	private:
		QueueData(const QueueData&) = delete;
		QueueData& operator = (const QueueData&) = delete;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class AbstractQueue;

	class __declspec(dllexport) QueueObject {
	public:
		//! @brief Return value for the activate call
		enum QueueResultFlag {
			Ok          = 0x00, //! @brief Everything ok, continue
			Requeue     = 0x01, //! @brief Add this object back to the queue with the same data
			CancelQueue = 0x02, //! @brief Cancel queue
			NoMemClear  = 0x04
		};
		typedef ot::Flags<QueueObject::QueueResultFlag> QueueResultFlags;

		QueueObject() {};
		virtual ~QueueObject() {};

		//! @brief Is called when this object is activated by the queue
		//! @param _args Arguments for the call
		virtual QueueResultFlags activateFromQueue(AbstractQueue* _queue, QueueData* _args) = 0;

	private:
		QueueObject(const QueueObject&) = delete;
		QueueObject& operator = (const QueueObject&) = delete;
	};
}

OT_ADD_FLAG_FUNCTIONS(ot::QueueObject::QueueResultFlag);

namespace ot {	

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class __declspec(dllexport) AbstractQueue {
	public:
		AbstractQueue() {};
		virtual ~AbstractQueue() {};

		//! @brief Add the provided object to the queue
		//! @param _obj The object to add to the queue
		//! @param _args The arguments object that should be provided to the objects activate call
		virtual void queue(QueueObject* _obj, QueueData * _args = (QueueData *)nullptr) noexcept = 0;

		//! @brief Add the provided object to the front of the queue
		//! @param _obj The object to add to the queue
		//! @param _args The arguments object that should be provided to the objects activate call
		virtual void queueNext(QueueObject* _obj, QueueData* _args = (QueueData*)nullptr) noexcept = 0;

		//! @brief Execute the queue
		virtual bool executeQueue(void) = 0;

	private:
		AbstractQueue(const AbstractQueue&) = delete;
		AbstractQueue& operator = (const AbstractQueue&) = delete;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class OT_CORE_API_EXPORT SimpleQueue : public AbstractQueue {
	public:
		SimpleQueue();
		virtual ~SimpleQueue();

		//! @brief Add the provided object to the queue
		//! @param _obj The object to add to the queue
		//! @param _args The arguments object that should be provided to the objects activate call
		virtual void queue(QueueObject* _obj, QueueData* _args = (QueueData*)nullptr) noexcept override;

		//! @brief Add the provided object to the front of the queue
		//! @param _obj The object to add to the queue
		//! @param _args The arguments object that should be provided to the objects activate call
		virtual void queueNext(QueueObject* _obj, QueueData* _args = (QueueData*)nullptr) noexcept override;

		//! @brief Execute the queue
		virtual bool executeQueue(void) override;

	protected:
		std::list<std::pair<QueueObject*, QueueData *>> m_queue;

	private:
		void clear(void);
		inline void memClear(const std::pair<QueueObject*, QueueData*>& _entry);

		SimpleQueue(const SimpleQueue&) = delete;
		SimpleQueue& operator = (const SimpleQueue&) = delete;
	};

}
