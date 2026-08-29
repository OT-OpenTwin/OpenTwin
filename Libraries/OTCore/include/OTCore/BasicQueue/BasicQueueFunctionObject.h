// @otlicense

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