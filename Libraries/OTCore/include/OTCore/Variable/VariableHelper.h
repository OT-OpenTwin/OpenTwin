// @otlicense

// OpenTwin header
#include "OTCore/Variable/Variable.h"
#include "OTCore/Serializable.h"

namespace ot {

	class VariableHelper
	{
	public:
		template <typename Op>
		static ot::Variable applyNumericOp(const ot::Variable& lhs, const ot::Variable& rhs, Op op)
		{
			if (lhs.isComplex() || rhs.isComplex())
			{
				return op(lhs.toComplex(), rhs.toComplex());
			}
			else if (lhs.isDouble() || rhs.isDouble())
			{
				return op(static_cast<double>(lhs.toDouble()), static_cast<double>(rhs.toDouble()));
			}
			else if (lhs.isFloat() || rhs.isFloat())
			{
				return op(static_cast<float>(lhs.toFloat()), static_cast<float>(rhs.toFloat()));
			}
			else if (lhs.isInt64() || rhs.isInt64())
			{
				return op(static_cast<int64_t>(lhs.toInt64()), static_cast<int64_t>(rhs.toInt64()));
			}
			else if (lhs.getInt32() && rhs.getInt32())
			{
				return op(lhs.getInt32(), rhs.getInt32());
			}
			else
			{
				throw std::exception("Numerical operation with non-numeric data types intended.");
			}
		}

		template <typename Op>
		static ot::Variable applyUnaryOp(const ot::Variable& operand, Op op)
		{
			if (operand.isComplex())
			{
				return op(operand.getComplex());
			}
			else if (operand.isDouble())
			{
				return op(operand.getDouble());
			}
			else if (operand.isFloat())
			{
				return op(operand.getFloat());
			}
			else if (operand.isInt64())
			{
				return op(operand.toDouble());  // promote: log/exp not defined for integers
			}
			else if (operand.isInt32())
			{
				return op(operand.toDouble());  // promote: same reason
			}
			else
			{
				throw std::runtime_error("Variable: unsupported type for unary op: " + operand.getTypeName());
			}
		}
	
		static ot::Variable ln(const ot::Variable& _variable)
		{
			return applyUnaryOp(_variable, [](auto v) -> ot::Variable
				{
					return std::log(v);
				});
		}

		static ot::Variable log10(const ot::Variable& _variable) 
		{
			return applyUnaryOp(_variable, [](auto v) -> ot::Variable
				{
					return std::log10(v);
				});
		}

		ot::Variable sqrt(const ot::Variable& _variable) 
		{
			return applyUnaryOp(_variable, [](auto v) -> ot::Variable
				{
					return std::sqrt(v);
				});
		}

		ot::Variable pow(const ot::Variable& _base, double _exponent) const
		{
			return applyNumericOp(_base, _exponent, [](auto base, auto exp) -> ot::Variable
				{
					return std::pow(base, exp);
				});
		}

	};
}