#pragma once
#ifndef KRIGING_PARAMETER_CLASS

#include <iostream>
#include <string>

using namespace std;
using std::string;

class KrigingParams {
public:
	KrigingParams() {}
	KrigingParams(KrigingParams* params) {
		theta = params->theta;
		nugget = params->nugget;
		corr = params->corr;
		poly = params->poly;
	}

	double theta = 0.01;
	double nugget = 2.220446049250313e-14;
	// Correlation function type = [�abs_exp�, �squar_exp�, �act_exp�, �matern52�, �matern32�] default = squar_exp
	string corr = "squar_exp"; //  ['abs_exp', 'squar_exp', 'matern52', 'matern32']
	// Regression function type = [�constant�, �linear�, �quadratic�] default = constant
	string poly = "linear";

	void printIt() {
		cout << "\n\nKriging Parameters: \n"
			<< "theta: \t\t" << theta
			<< "\n"
			<< "nugget: \t" << nugget
			<< "\n"
			<< "corr: \t\t" << corr
			<< "\n"
			<< "poly: \t\t" << poly
			<< "\n\n";
	};

	std::string toString() {
		return "\n\nKriging Parameters: \ntheta: \t\t" + std::to_string(theta)
			+ "\n"
			+ "nugget: \t" + std::to_string(nugget)
			+ "\n"
			+ "corr: \t\t" + corr
			+ "\n"
			+ "poly: \t\t" + poly
			+ "\n\n";
	};
};

#endif