// @otlicense

#include "FixtureIntrinsics.h"

FixtureIntrinsics::FixtureIntrinsics()
{
	
		_a = static_cast<float*>(_aligned_malloc(_vectorSize * sizeof(float), _alignment));
		_b = static_cast<float*>(_aligned_malloc(_vectorSize * sizeof(float), _alignment));
		for (int i = 0; i < _vectorSize; i++)
		{
			_a[i] = i;
			_b[i] = (i + 1);
		}
}

FixtureIntrinsics::~FixtureIntrinsics()
{
	_aligned_free(_a);
	_aligned_free(_b);
}
