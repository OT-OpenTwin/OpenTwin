//! @file BasicScopedBoolWrapper.h
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"	

namespace ot {

	//! @brief Basic lifecycle management for a boolean value that should be set on wrapper creation and reset when leaving the scope.
	//! Based on the <a href="https://en.cppreference.com/w/cpp/language/raii">RAII pattern</a>.
    //! 
    //! Example:
    //! 
    //! @code
    //! #include "OTCore/BasicScopedBoolWrapper.h"
    //! 
    //! bool myBool = false;
    //! 
    //! void foo(void) {
    //!     // assume myBool = false;
    //! 
    //!     ot::BasicScopedBoolWrapper boolWrap1(myBool);
    //! 
    //!     // myBool       = true;  -> boolWrap1()
    //! 
    //!     try {
    //!         ot::BasicScopedBoolWrapper boolWrap2(myBool, false, true);
    //! 
    //!         // myBool   = false; -> boolWrap2()
    //! 
    //!         throw 0;
    //! 
    //!     } catch (...) {
    //!         std::cout << "Oh no!\n";
    //!     }
    //! 
    //!     // myBool       = true;  -> ~boolWrap2()
    //! 
    //!     ...
    //! }
    //! 
    //! // myBool           = false; -> ~boolWrap1()
    //! @endcode
	class BasicScopedBoolWrapper {
		OT_DECL_NOCOPY(BasicScopedBoolWrapper)
		OT_DECL_NOMOVE(BasicScopedBoolWrapper)
		OT_DECL_NODEFAULT(BasicScopedBoolWrapper)
	public:
		//! @brief Constructor.
		//! Will call BasicScopedBoolWrapper(bool& _value, bool _set, bool _reset) by providing the _value agrgument as reset argument aswell.
		//! @param _value Value reference will be stored and has to remain valid while the wrapper instance is not destroyed.
		//! @param _set The _set value will be applied to the referenced _value.
		BasicScopedBoolWrapper(bool& _value, bool _set = true) : BasicScopedBoolWrapper(_value, _set, _value) {};

		//! @brief Constructor.
		//! @param _value Value reference will be stored and has to remain valid while the wrapper instance is not destroyed..
		//! @param _set The _set value will be applied to the referenced _value.
		//! @param _reset A copy of _reset will be stored and applied to the BasicScopedBoolWrapper#m_value upon destruction of the wrapper.
		BasicScopedBoolWrapper(bool& _value, bool _set, bool _reset) : m_value(_value = _set), m_reset(_reset) {}

		//! @brief Destructor.
		//! Will apply the reset value to the referenced value.
		~BasicScopedBoolWrapper() {
			m_value = m_reset;
		}

	private:
		bool& m_value; //! @brief Value reference.
		bool m_reset; //! @brief Reset value to apply in the destructor.

	};
}