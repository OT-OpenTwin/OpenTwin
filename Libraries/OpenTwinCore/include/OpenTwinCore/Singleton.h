//! @file Singleton.h
//! @brief Defines the Singleton class template.
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#ifndef OT_SINGLETON_H
#define OT_SINGLETON_H

#include "OpenTwinCore/CoreAPIExport.h"

//! @brief Required macro for classes implementing the Singleton interface
//! "The class that is provided as T should add this macro to their class declaration"
//! Provide the Singleton type as arg(s)
#define OT_SINGLETON(...) friend class ot::Singleton<__VA_ARGS__>;

namespace ot {

	//! @brief Default singleton template
	//! "The class that is provided as T should add the OT_SINGLETON macro to their class declaration"
	template <class T> class OT_CORE_API_EXPORTONLY Singleton {
	public:
		//! @brief Returns the single instance of the class.
		//! This method provides access to the single instance of the template parameter class.
		//! If the instance does not exist yet, it will be created.
		//! @return A reference to the single instance of the template parameter class.
		static T& instance() {
			static T instance;
			return instance;
		}

	protected:
		//! @brief Protected constructor to prevent creation of multiple instances.
		Singleton() = default;

		//! @brief Protected destructor to prevent deletion of the single instance.
		virtual ~Singleton() = default;

	private:

		//! @brief Private copy constructor to prevent copying of the singleton.
		Singleton(const Singleton&) = delete;

		//! @brief Private assignment operator to prevent assignment of the singleton.
		Singleton& operator = (const Singleton&) = delete;
	};

}

#endif