// @otlicense

#pragma once
#include <stdint.h>
#include "OTCore/Variable.h"

namespace ts
{
	namespace option
	{
		enum class Frequency : uint32_t
		{
			Hz = 0,
			kHz = 1,
			MHz = 2,
			GHz = 3
		};

		enum class Format : uint32_t
		{
			Decibel_angle = 0,
			magnitude_angle = 1,
			real_imaginary = 2
		};

		enum class Parameter : uint32_t
		{
			Scattering = 0,
			Admittance = 1,
			Impedance = 2,
			Hybrid_h = 3,
			Hybrid_g = 4
		};
	}
}
