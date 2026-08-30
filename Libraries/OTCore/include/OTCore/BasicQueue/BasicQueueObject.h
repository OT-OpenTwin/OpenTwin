// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/CoreTypes.h"

namespace ot
{

	class BasicQueue;

	//! @brief The BasicQueueObject class is the base class for all objects that can be pushed to the BasicQueue.
	//! The class provides the necessary interface for executing the object and handling its unique properties in the queue.
	class OT_CORE_API_EXPORT BasicQueueObject
	{
		OT_DECL_NOCOPY(BasicQueueObject)
		OT_DECL_NOMOVE(BasicQueueObject)
		OT_DECL_NODEFAULT(BasicQueueObject)
	public:
		enum InsertOrder
		{
			InsertBack,  //! @brief Inserts the object at the back of the queue.
			InsertFront  //! @brief Inserts the object at the front of the queue.
		};

		enum UniqueInsertBahavior
		{
			KeepExisting,    //! @brief Keeps the original object in the queue and ignores the new. If no original object is found the new one will be inserted according to its insert order.
			ReplaceExisting, //! @brief Replaces the original object at its position in the queue with the new one. If no original object is found the new one will be inserted according to its insert order.
			RemoveExisting   //! @brief Removes the original object from the queue and pushes the new one according to the insert order of the new object.
		};

		//! @brief Creates a new BasicQueueObject.
		//! @param _insertOrder Defines the insert order of the object in the queue. The default is to insert the object at the back of the queue.
		explicit BasicQueueObject(InsertOrder _insertOrder);
		virtual ~BasicQueueObject();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Mandatory methods

		//! @brief Returns the class name of the object.
		virtual std::string getClassName() const = 0;

		//! @brief Executes the object.
		//! This method will be called by the queue when the object is popped from the queue.
		virtual int exec() = 0;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Queue handling

		//! @brief Stops the execution of the object.
		//! This method will be called by the queue when stop() is called.
		//! The object should stop its execution as soon as possible and return from the exec() method.
		//! The default implementation does nothing.
		virtual void stopQueueObjectExecution() {};

		//! @brief Marks the object as unique.
		//! Unique objects with the same key may only exist a single time at once in the queue.
		//! If no key is set the class name will be used as key.
		//! This must be called before the object is pushed to the queue.
		//! @param _uniqueInsertBehavior Defines how the queue should handle this unique object when inserting it into the queue. The default is to remove the original object from the queue and push the new one according to the insert order of the new object.
		BasicQueueObject* makeQueueUnique(UniqueInsertBahavior _uniqueInsertBehavior = UniqueInsertBahavior::RemoveExisting);

		//! @brief Marks the object as unique with a custom key.
		//! Unique objects with the same key may only exist a single time at once in the queue.
		//! This must be called before the object is pushed to the queue.
		//! @param _customKey The key to use for this unique object. If no key is set the class name will be used as key.
		//! @param _uniqueInsertBehavior Defines how the queue should handle this unique object when inserting it into the queue. The default is to remove the original object from the queue and push the new one according to the insert order of the new object.
		BasicQueueObject* makeQueueUnique(const std::string& _customKey, UniqueInsertBahavior _uniqueInsertBehavior = UniqueInsertBahavior::RemoveExisting);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Getter

		//! @brief Returns the key of the unique object.
		//! If no key was set the class name will be used as key.
		std::string getQueueObjectKey() const { return m_customKey.empty() ? this->getClassName() : m_customKey; };

		//! @brief Returns the insert order of the object.
		InsertOrder getInsertOrder() const { return m_insertOrder; };

		//! @brief Returns true if the object is marked as unique.
		bool isQueueObjectUnique() const { return m_isUnique; };

		//! @brief Returns the unique insert type of the object.
		UniqueInsertBahavior getUniqueInsertBehavior() const { return m_uniqueInsertBehavior; };

	private:
		friend class BasicQueue;

		InsertOrder m_insertOrder;
		bool m_isUnique;
		std::string m_customKey;
		UniqueInsertBahavior m_uniqueInsertBehavior;
	};

}