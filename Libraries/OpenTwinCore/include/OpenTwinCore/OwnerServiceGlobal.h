#pragma once
#include "OpenTwinCore/OwnerService.h"

namespace ot
{
	class __declspec(dllexport) OwnerServiceGlobal : public OwnerService {
	public:
		static OwnerServiceGlobal& instance(void);


	private:
		OwnerServiceGlobal() : OwnerService() {};
		virtual ~OwnerServiceGlobal() {};

		OwnerServiceGlobal(const OwnerServiceGlobal&) = delete;
		OwnerServiceGlobal& operator = (const OwnerServiceGlobal&) = delete;
	};
}
