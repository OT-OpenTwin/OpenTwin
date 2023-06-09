#pragma once

class MeasuredTargetValuesSP
{
private:
	const float uniformGridTimeDiscretization = 9.43936818e-10f;
	const float uniformGridCoefficientValue = 0.00150232145f;
	const float uniformGridDualCoefficientValue = 213.092438f;
	const float portHardwiredGaussianT0 = 0.000123409802f;
	const float portHardwiredGaussianT1 = 0.000222629897f;
	const float portHardwiredGaussianT2 = 0.000393668975f;
	
	const int intrinsicIncludeValue = 1;
	const int intrinsicExcludeValue = 0;

	MeasuredTargetValuesSP() {};


public:
	static MeasuredTargetValuesSP& GetInstance()
	{
		static MeasuredTargetValuesSP instance;
		return instance;
	}

	float GetUniformGridTimeDiscretization() const { return uniformGridTimeDiscretization; };
	float GetUniformGridCoefficientValue() const { return uniformGridCoefficientValue; };
	float GetUniformGridDualCoefficientValue() const { return uniformGridDualCoefficientValue; };
	float GetPortHardwiredGaussianT0() const { return portHardwiredGaussianT0; };
	float GetPortHardwiredGaussianT1() const { return portHardwiredGaussianT1; };
	float GetPortHardwiredGaussianT2() const { return portHardwiredGaussianT2; };
	int GetIntrinsicIncludeValue() const { return intrinsicIncludeValue; };
	int GetIntrinsicExcludeValue()const { return intrinsicExcludeValue; };
};

static MeasuredTargetValuesSP _measuredTargetValuesSP = MeasuredTargetValuesSP::GetInstance();