//! @file Queue.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/CoreAPIExport.h"
#include "OTCore/Flags.h"

// std header
#include <list>

#pragma warning (disable:4251)

namespace ot {

	class __declspec(dllexport) QueueDestroyableObject {
	public:
		//! @brief Describes how the object will be destroyed after
		enum CleanupFlag {
			DeleteAtEnd = 0x00, //! @brief Queue keeps ownership. Delete the object when the queue will be cleaned
			DeleteAfter = 0x01, //! @brief Queue keeps ownership. Delete the data object after the the execution of the call rather than after the whole queue
			NoDelete    = 0x03, //! @brief Creator keeps ownership. Data object won't be deleted when the queue will perform its garbage collection clear (overrides all)
			Multiuse    = 0x04  //! @brief Must be set if the object is passed to the queue multilple times (a requeue does not count as multiuse)
		};
		typedef Flags<CleanupFlag> CleanupFlags;
				
		QueueDestroyableObject() : m_cleanupFlags(DeleteAtEnd) {};
		QueueDestroyableObject(CleanupFlag _cleanupFlags) : m_cleanupFlags(_cleanupFlags) {};
		virtual ~QueueDestroyableObject() {};

		void setCleanupFlag(CleanupFlag _flag, bool _active = true) { m_cleanupFlags.setFlag(_flag, _active); };
		void setCleanupFlags(const CleanupFlags& _flags) { m_cleanupFlags = _flags; };
		const CleanupFlags& cleanupFlags(void) const { return m_cleanupFlags; };

	private:
		CleanupFlags m_cleanupFlags;

		QueueDestroyableObject(const QueueDestroyableObject&) = delete;
		QueueDestroyableObject& operator = (const QueueDestroyableObject&) = delete;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class __declspec(dllexport) QueueData :  public QueueDestroyableObject {
	public:
		QueueData(CleanupFlag _cleanupFlags = DeleteAtEnd) : QueueDestroyableObject(_cleanupFlags) {};
		virtual ~QueueData() {};

	private:
		
		QueueData(const QueueData&) = delete;
		QueueData& operator = (const QueueData&) = delete;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class AbstractQueue;

	class __declspec(dllexport) QueueObject : public QueueDestroyableObject {
	public:
		//! @brief Return value for the activate call
		enum QueueResultFlag {
			Ok          = 0x00, //! @brief Everything ok, continue
			JobFailed   = 0x01, //! @brief Execution failed, ignore and continue
			Requeue     = 0x02, //! @brief Add this object back to the queue with the same data
			CancelQueue = 0x04, //! @brief Exit the queue (now) with error
			FinishQueue = 0x08  //! @brief Exit the queue (now) with ok
		};
		typedef ot::Flags<QueueObject::QueueResultFlag> QueueResultFlags;

		QueueObject(CleanupFlag _cleanupFlags = DeleteAtEnd) : QueueDestroyableObject(_cleanupFlags) {};
		virtual ~QueueObject() {};

		//! @brief Is called when this object is activated by the queue
		//! @param _args Arguments for the call
		virtual QueueResultFlags activateFromQueue(AbstractQueue* _queue, QueueData* _args) = 0;

	private:
		QueueObject(const QueueObject&) = delete;
		QueueObject& operator = (const QueueObject&) = delete;
	};
}

OT_ADD_FLAG_FUNCTIONS(ot::QueueDestroyableObject::CleanupFlag)
OT_ADD_FLAG_FUNCTIONS(ot::QueueObject::QueueResultFlag)

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
		//! @param _arg The arguments object that will be passed to the objects activate call
		virtual void queue(QueueObject* _obj, QueueData* _arg = (QueueData*)nullptr) noexcept override;

		//! @brief Add the provided object to the front of the queue
		//! @param _obj The object to add to the queue
		//! @param _arg The arguments object that will be passed to the objects activate call
		virtual void queueNext(QueueObject* _obj, QueueData* _arg = (QueueData*)nullptr) noexcept override;

		//! @brief Execute the queue
		virtual bool executeQueue(void) override;

		//! @brief Clear the queue
		void clearQueue(void);

	protected:

		//! @brief Will clear the memory according to the objects cleanup flags
		//! @note Note that the provided object might be destroyed after calling this function
		void checkAndDestroy(QueueDestroyableObject* _obj);

	private:
		void clearGarbage(void);

		std::list<std::pair<QueueObject*, QueueData*>> m_queue;
		std::list<std::pair<QueueDestroyableObject*, bool>> m_garbage;
		unsigned int m_garbageCounter;

		SimpleQueue(const SimpleQueue&) = delete;
		SimpleQueue& operator = (const SimpleQueue&) = delete;
	};

}
