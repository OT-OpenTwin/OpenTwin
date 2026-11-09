// @otlicense
// File: BasicQueueFunctionObject.h
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
#include "OTCore/BasicQueue/BasicQueueObject.h"

// std header
#include <utility>
#include <type_traits>

namespace ot
{

    template <typename Callable>
	class BasicQueueFunctionObject : public BasicQueueObject
    {
		OT_DECL_NOCOPY(BasicQueueFunctionObject)
		OT_DECL_NOMOVE(BasicQueueFunctionObject)
        OT_DECL_NODEFAULT(BasicQueueFunctionObject)
    public:
        BasicQueueFunctionObject(Callable _callable, InsertOrder _insertOrder = InsertOrder::InsertBack)
            : BasicQueueObject(_insertOrder), m_callable(std::move(_callable))
        {
            static_assert(
                std::is_convertible_v<std::invoke_result_t<Callable>, int>,
                "Callable must be invocable without arguments and return something convertible to int"
                );
        }

        static std::string className() { return "BasicQueueFunctionObject"; };
        virtual std::string getClassName() const override { return BasicQueueFunctionObject::className(); };
        
        virtual int exec() override
        {
            return static_cast<int>(m_callable());
        }

    private:
        Callable m_callable;
    };


}