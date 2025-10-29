// @otlicense

//! @file BasicNumberIncrementWrapper.h
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/BasicRAIIWrapper.h"

namespace ot {

    //! @class BasicNumberIncrementWrapper
    //! @brief Wrapper increments a value on creation and decrements the value when the instance is destroyed.
    //! Based on the <a href="https://en.cppreference.com/w/cpp/language/raii">RAII pattern</a>.
    //! 
    //! Example:
    //! 
    //! @code
    //! #include "OTCore/BasicNumberIncrementWrapper.h"
    //! 
    //! int myValue = 0;
    //! 
    //! void foo(void) {
    //!     // assume myValue = 0;
    //! 
    //!     ot::BasicNumberIncrementWrapper<int> numWrap1(myNumber);
    //! 
    //!     // myValue = 1;
    //! 
    //!     try {
    //!         ot::BasicNumberIncrementWrapper<int> numWrap2(myNumber);
    //! 
    //!         // myValue = 2;
    //! 
    //!         throw 0;
    //! 
    //!     } catch (...) {
    //!         std::cout << "Oh no!\n";
    //!     }
    //! 
    //!     // myValue = 1;
    //! 
    //!     ...
    //! }
    //! 
    //! // myValue = 0 after leaving foo();
    //! @endcode
    //! @tparam T Must support the ++ and -- operator.
    template <typename T> class BasicNumberIncrementWrapper : public BasicRAIIWrapper<T> {
        OT_DECL_NOCOPY(BasicNumberIncrementWrapper)
        OT_DECL_NOMOVE(BasicNumberIncrementWrapper)
        OT_DECL_NODEFAULT(BasicNumberIncrementWrapper)
    public:
        //! @brief Constructor.
        //! @param _value Value reference will be stored and has to remain valid while the wrapper instance is not destroyed.
        //! @ref BasicRAIIWrapper::BasicRAIIWrapper(T& _value, std::function<void(T&)> _onEnter, std::function<void(T&)> _onExit)
        BasicNumberIncrementWrapper(T& _value) : BasicRAIIWrapper<T>(
            _value,
            [](T& _lcl) { static_cast<T&>(_lcl)++; }, // Static cast ensures the ++ operator can be used here.
            [](T& _lcl) { static_cast<T&>(_lcl)--; }  // Static cast ensures the -- operator can be used here.
        ){};

        virtual ~BasicNumberIncrementWrapper() = default;
    };

}
