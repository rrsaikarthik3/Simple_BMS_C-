#pragma once
#include <iostream>
#include <thread>
#include <vector>
#include <Eigen/Dense>
#include "cell_param.h"
using Eigen::MatrixXd;
using Eigen::VectorXd;


enum module_type
{
	parallel,
	series
};


class module
{
public:
	module();
	module(double _v, double _c, double _z, int n) : volt_m(_v), curr_m(_c), soc_m(_z), mod_no(n){ Init(); }
	~module();
	void Init();
	void set_volt(double v);
	void set_curr(double i);
	double get_volt();
	double get_curr();
	double get_soc();
	void ekf_soc_predict();
	void ekf_soc_update();
	double get_eff_ro(){ return eff_int_r0; }
	double get_eff_cap() { return Q_m; }
	void update_cell_param(double T, std::shared_ptr<cell_param> cp, int n_o_c, module_type mod);

private:
	module_type Mod_arr = parallel;
	int mod_no;

	double volt_m, soc_m, ocv, curr_m;
	int count_bad = 0;
	double eff_int_r0, eff_r1, eff_r2, eff_c1, eff_c2, dOCV_Z,Q_m, time_c1, time_c2;
	Eigen::MatrixXd A_hat,C_hat, D_hat, Sigma_X, Sigma_W, B_hat;
	Eigen::VectorXd x_, Sigma_Y;
	Eigen::VectorXd Sigma_V ;

};

