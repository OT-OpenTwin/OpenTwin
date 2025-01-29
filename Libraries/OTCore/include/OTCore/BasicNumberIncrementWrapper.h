//! @file BasicNumberIncrementWrapper.h
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/BasicRAIIWrapper.h"

namespace ot {

    template <typename T>
    class BasicNumberIncrementWrapper : public BasicRAIIWrapper<T> {
    public:
        BasicNumberIncrementWrapper() = delete;
        BasicNumberIncrementWrapper(T& _value) : BasicRAIIWrapper<T>(_value, [](T& _lcl) {_lcl++; }, [](T& _lcl) {_lcl--; }) {};

        BasicNumberIncrementWrapper(const BasicNumberIncrementWrapper&) = delete;
        BasicNumberIncrementWrapper& operator = (const BasicNumberIncrementWrapper&) = delete;
        BasicNumberIncrementWrapper(BasicNumberIncrementWrapper&&) = default;
        BasicNumberIncrementWrapper& operator = (BasicNumberIncrementWrapper&&) = default;
    };

}
