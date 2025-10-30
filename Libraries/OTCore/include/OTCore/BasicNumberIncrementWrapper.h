// @otlicense
// File: BasicNumberIncrementWrapper.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

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
