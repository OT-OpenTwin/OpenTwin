// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/CoreTypes.h"

namespace ot
{

	class OT_CORE_API_EXPORT BasicQueueObject
	{
		OT_DECL_NOCOPY(BasicQueueObject)
		OT_DECL_NOMOVE(BasicQueueObject)
	public:
		enum InsertOrder
		{
			InsertBack,  //! @brief Inserts the object at the back of the queue.
			InsertFront  //! @brief Inserts the object at the front of the queue.
		};

		enum UniqueQueueInsertType
		{
			SkipUnique,       //! @brief Ignores the new object and keeps the original in the queue.
			KeepUnique,       //! @brief Keeps the original object in the queue and ignores the new.
			ReplaceUnique,    //! @brief Replaces the original object in the queue with the new one.
			ReplaceUniqueBack //! @brief Replaces the original object in the queue with the new one and moves it to the back of the queue.
		};

		//! @brief Creates a new BasicQueueObject.
		//! @param _insertOrder Defines the insert order of the object in the queue. The default is to insert the object at the back of the queue.
		BasicQueueObject(InsertOrder _insertOrder = InsertOrder::InsertBack);
		virtual ~BasicQueueObject();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Mandatory methods for inheriting classes

		//! @brief Returns the class name of the object.
		virtual std::string getClassName() const = 0;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Queue handling

		//! @brief Marks the object as unique.
		//! Unique objects with the same key may only exist a single time at once in the queue.
		//! If no key is set the class name will be used as key.
		//! This must be called before the object is pushed to the queue.
		//! @param _uniqueInsertType Defines how the queue should handle this unique object when inserting it into the queue. The default is to replace the original object with the new one and move it to the back of the queue.
		BasicQueueObject* makeQueueUnique(UniqueQueueInsertType _uniqueInsertType = UniqueQueueInsertType::ReplaceUniqueBack);

		//! @brief Marks the object as unique with a custom key.
		//! Unique objects with the same key may only exist a single time at once in the queue.
		//! This must be called before the object is pushed to the queue.
		//! @param _customKey The key to use for this unique object. If no key is set the class name will be used as key.
		//! @param _uniqueInsertType Defines how the queue should handle this unique object when inserting it into the queue. The default is to replace the original object with the new one and move it to the back of the queue.
		BasicQueueObject* makeQueueUnique(const std::string& _customKey, UniqueQueueInsertType _uniqueInsertType = UniqueQueueInsertType::ReplaceUniqueBack);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Getter

		//! @brief Returns true if the object is marked as unique.
		bool isQueueObjectUnique() const { return m_isUnique; };

		//! @brief Returns the key of the unique object.
		//! If no key was set the class name will be used as key.
		std::string getQueueObjectKey() const { return m_customKey.empty() ? this->getClassName() : m_customKey; };

		//! @brief Returns the insert order of the object.
		InsertOrder getInsertOrder() const { return m_insertOrder; };

	private:
		InsertOrder m_insertOrder;
		bool m_isUnique;
		std::string m_customKey;
		UniqueQueueInsertType m_uniqueInsertType;
	};

}