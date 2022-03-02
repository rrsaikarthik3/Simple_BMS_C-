#pragma once
#include <vector>
#include <Eigen/Dense>
using std::vector;
using Eigen::MatrixXd;
using Eigen::VectorXd;

/*
This class stores the Battery cell data for a Standard Li Ion cell. This can be modified based on the cell 
chemistry chosed. Here,  a simple equivalent cell model is chosen with one Internal Reistor and one RC Pair
to mimick the actuall Battery cell behavior.

This class also helps in accessing the Battery Data that will be used by Module class mainly for state estimation
*/


class cell_param
{
private:
 // OCV fn(SOC, Temp)
	vector<double> SOC_brk_pt = { 0,0.1,0.25,0.5,0.75,0.9,1.0 };  //Range of SOCs, the data is present for
	vector<double> Temp_brk_pt = { 5, 20, 40 }; //Range of Temps, the data is present for
	Eigen::MatrixXd OCV_tbl;
	double min_rated_v = 3.3;
	double max_rated_v = 4.2;
	double min_rated_soc = 0.08;
	double max_rated_soc = 0.95;
	vector<double> R0, R1, C1, dOCV_dz; // Internal Resistor and RC Pair values as fn(Temp)  - SOC dependence is considered negligible here
	double Q0; //Initial Battery capacity which is assumed to be constant for now. This will be later updated in SOH estimation

public:
	double get_ocv_from_soc(double z, double T);
	double get_R0_from_temp(double T);
	double get_R1_from_temp(double T);
	double get_C1_from_temp(double T);
	double get_dOCV_dz_from_temp(double T);
	double get_min_volt() { 
		return min_rated_v; 
	}
	double get_max_volt() {
		return max_rated_v;
	}
	double get_min_soc() {
		return min_rated_soc;
	}
	double get_max_soc() {
		return max_rated_soc;
	}
	vector<int> get_minmaxind(double x , vector<double> Arr);
	double get_x_from_temp(double T, vector<double> p);

	double get_Cap();
	cell_param();
	~cell_param();


};

