#pragma once
#include <vector>
namespace ot
{
	struct __declspec(dllexport) ComplexNumberContainer
	{
		virtual ~ComplexNumberContainer()=default;
	};

	struct __declspec(dllexport) ComplexNumberContainerPolar : public ComplexNumberContainer
	{
		~ComplexNumberContainerPolar() override = default;
		std::vector<double> m_magnitudes;
		std::vector<double> m_phases;

	};

	struct __declspec(dllexport) ComplexNumberContainerCartesian : public ComplexNumberContainer
	{
		~ComplexNumberContainerCartesian() override = default; 
		std::vector<double> m_real;
		std::vector<double> m_imag;
	};
}
