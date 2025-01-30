//! @file BasicRAIIWrapper.h
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"

// std header
#include <functional>

namespace ot {

    //! @brief The BasicRAIIWrapper may be used to impletement any basic lifecycle management for a given object.
    //! Based on the <a href="https://en.cppreference.com/w/cpp/language/raii">RAII pattern</a>. <br>
    //! 
    //! Example:
    //! 
    //! @code
    //! #include "OTCore/BasicRAIIWrapper.h"
    //! 
    //! void foo(void) {
    //!     int* myNumber = nullptr;
    //!     ot::BasicRAIIWrapper<int*> num(
    //!         myNumber,                 // Will be stored and passed to the two provided methods.
    //!         [](int*& _lcl){           // When creating "num" myNumber will be initialized with a new pointer.
    //!             _lcl = new int;
    //!             assert(_lcl != nullptr);
    //!         },
    //!         [](int*&){ delete _lcl; } // The destructor will destroy myNumber.
    //!     );
    //! 
    //!     // Now the value was created.
    //! 
    //!     // Do anything with myNumber or use num.getValue() to access the pointer.
    //!     ...
    //! }
    //! @endcode
    template <typename T> class BasicRAIIWrapper {
        OT_DECL_NOCOPY(BasicRAIIWrapper)
        OT_DECL_NOMOVE(BasicRAIIWrapper)
        OT_DECL_NODEFAULT(BasicRAIIWrapper)
    public:
        //! @brief Constructor.
        //! @param _value Value reference will be stored and has to remain valid while the wrapper instance is not destroyed.
        //! @param _onEnter Function that should be called when the wrapper instance is created.
        //! @param _onExit Function that should be called when the wrapper instance is destroyed.
        BasicRAIIWrapper(T& _value, std::function<void(T&)> _onEnter, std::function<void(T&)> _onExit)
            : m_value(_value), m_onExit(_onExit)
        {
            if (_onEnter) {
                _onEnter(m_value);
            }
        }

        //! @brief Destructor.
        //! If the BasicRAIIWrapper#m_onExit function was set it will be called.
        virtual ~BasicRAIIWrapper() {
            if (m_onExit) {
                m_onExit(m_value);
            }
        }

        T& getValueRef(void) const { return m_value; };

    private:
        T& m_value; //! @brief Value reference.
        std::function<void(T&)> m_onExit; //! @brief Function that will be called in the destructor.

    };

}
