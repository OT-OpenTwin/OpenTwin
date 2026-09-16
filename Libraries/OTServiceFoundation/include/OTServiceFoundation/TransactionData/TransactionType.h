#pragma once
#include <cassert>
#include "OTServiceFoundation/FoundationAPIExport.h"

namespace ot
{
	enum class OT_SERVICEFOUNDATION_API_EXPORT TransactionType
	{
		Import,
		Execute_Event,
		Archive_Data,
		Execute_Engine,
		Refinement,
		UNKNOWN
	};

	namespace Transaction
	{
		static int toInt(TransactionType _transactionType)
		{
			if (_transactionType == TransactionType::UNKNOWN)
			{
				return 0;
			}
			else if (_transactionType == TransactionType::Import)
			{
				return 1;
			}
			else if (_transactionType == TransactionType::Execute_Event)
			{
				return 2;
			}
			else if (_transactionType == TransactionType::Archive_Data)
			{
				return 3;
			}
			else if (_transactionType == TransactionType::Execute_Engine)
			{
				return 4;
			}
			else if (_transactionType == TransactionType::Refinement)
			{
				return 5;
			}
			else
			{
				assert(0); // Missing implementation !
				return -1;
			}
		}
		static TransactionType fromInt(int _transactionType)
		{
			if (_transactionType == 0)
			{
				return TransactionType::UNKNOWN;
			}
			else if (_transactionType == 1)
			{
				return TransactionType::Import;
			}
			else if (_transactionType == 2)
			{
				return TransactionType::Execute_Event;
			}
			else if (_transactionType == 3)
			{
				return TransactionType::Archive_Data;
			}
			else if (_transactionType == 4)
			{
				return TransactionType::Execute_Engine;
			}
			else if (_transactionType == 5)
			{
				return TransactionType::Refinement;
			}
			else
			{
				assert(false); // Missing implementation
				return TransactionType::UNKNOWN;
			}
		}

	}
}
