//! @file CopyInformationFactory.h
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/FactoryTemplate.h"
#include "OTCore/CopyInformation.h"

namespace ot {

	class OT_CORE_API_EXPORT CopyInformationFactory : public FactoryTemplate<CopyInformation> {
		OT_DECL_NOCOPY(CopyInformationFactory)
		OT_DECL_NOMOVE(CopyInformationFactory)
	public:
		static CopyInformationFactory& instance(void);

		static CopyInformation* create(const ConstJsonObject& _object);

	private:
		CopyInformationFactory() {};
		virtual ~CopyInformationFactory() {};
	};

	template <class T>
	class OT_CORE_API_EXPORTONLY CopyInformationFactoryRegistrar : public FactoryRegistrarTemplate<CopyInformationFactory, T> {
		OT_DECL_NOCOPY(CopyInformationFactoryRegistrar)
		OT_DECL_NODEFAULT(CopyInformationFactoryRegistrar)
	public:
		CopyInformationFactoryRegistrar(const std::string& _key) : FactoryRegistrarTemplate<CopyInformationFactory, T>(_key) {};
	};

}
