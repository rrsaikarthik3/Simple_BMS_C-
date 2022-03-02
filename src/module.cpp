#include "module.h"
#include "cell_param.h"
#include <cmath>
double deltaT = 0.100; //100ms
module::module()
{
	Init();
}
void module::set_volt(double v)
{
	volt_m = v;
}

void module::set_curr(double i)
{
	curr_m = i;	
}
double module::get_soc()
{
	return soc_m;
}
double module::get_volt()
{
	return volt_m;
}
double module::get_curr()
{
	return curr_m;
}
void module::ekf_soc_predict()
{
	x_ =  A_hat * x_ + B_hat*curr_m;
	Sigma_X = A_hat * Sigma_X * A_hat.transpose() + Sigma_W;

	if (abs(x_[0] - soc_m) < 0.1 && x_[0] > 0.05 && x_[0] < 0.95) // sanity check so that out of bound results are not recorded
	{
		soc_m = x_[0];
	}

}
void module::ekf_soc_update()
{

	double volt_pred = ocv - x_[1] * eff_r1 - x_[2] * eff_r2 - eff_int_r0 * curr_m;	
	double volt_err = volt_m - volt_pred;

	if (pow(volt_err, 2) > 4*Sigma_Y[0])
	{
		//bad sensor measurement 
		count_bad++;
		return;
	}
	if (count_bad >= 50)
	{
		std::cout << "\n Sensor Fault!!!";
	}
	
	Sigma_Y = C_hat * Sigma_X * C_hat.transpose() + D_hat*Sigma_V*(D_hat.transpose());
	
	MatrixXd L = Sigma_X * (C_hat.transpose()) * (Sigma_Y.inverse());

	x_ = x_ + (L * volt_err);
	Sigma_X = Sigma_X - L * Sigma_Y * L.transpose(); 

	if (abs(x_[0] - soc_m) < 0.051 && x_[0] > 0.05 && x_[0] < 0.95)
	{
		soc_m = x_[0];
	}
	soc_m = x_[0];


}
void module::update_cell_param(double T, std::shared_ptr<cell_param> cp, int n_o_c, module_type mod)
{
	if (mod == module_type::parallel)
	{
		eff_int_r0 = cp->get_R0_from_temp(T) / n_o_c;
		eff_r1 = cp->get_R1_from_temp(T) / n_o_c;
		eff_r2 = cp->get_R1_from_temp(T) / n_o_c;  // Potentially this could be a different value, since the data was not available, it is being considered to have same value as R1
		eff_c1 = cp->get_C1_from_temp(T) * n_o_c;
		eff_c2 = cp->get_C1_from_temp(T) * n_o_c;
		time_c1 = abs(1/(eff_r1 * eff_c1));
		time_c2 = abs(1 / (eff_r2 * eff_c2));
		Q_m = n_o_c*cp->get_Cap();
		ocv = cp->get_ocv_from_soc(soc_m, T);
		dOCV_Z = cp->get_dOCV_dz_from_temp(T);


	}
	else if (mod == module_type::series)
	{
		
		eff_int_r0 = cp->get_R0_from_temp(T) * n_o_c;
		eff_r1 = cp->get_R1_from_temp(T) * n_o_c;
		eff_r2 = cp->get_R1_from_temp(T) * n_o_c;  // Potentially this could be a different value, since the data was not available, it is being considered to have same value as R1
		eff_c1 = cp->get_C1_from_temp(T) / n_o_c;
		eff_c2 = cp->get_C1_from_temp(T) / n_o_c;
		time_c1 = abs(1 / (eff_r1 * eff_c1));
		time_c2 = abs(1 / (eff_r2 * eff_c2));
		Q_m = n_o_c * cp->get_Cap();
		ocv = n_o_c*cp->get_ocv_from_soc(soc_m, T);
		dOCV_Z = cp->get_dOCV_dz_from_temp(T)* n_o_c;
	}
	
	double tau1 = exp(-deltaT / time_c1);
	double tau2 = exp(-deltaT / time_c2);
	A_hat << 1, 0, 0,
		0, tau1, 0,
		0, 0, tau2;
	B_hat << -deltaT / (3600 * Q_m), 1 - tau1, 1 - tau2;
	
	C_hat << dOCV_Z, -eff_r1, -eff_r2;
	D_hat << 1;



}
void module::Init()
{

	Sigma_V = VectorXd(1);
	Sigma_V << .04;
	Sigma_W = MatrixXd(3, 3);
	Sigma_W << 0.0001, 0, 0,
		0, 1, 0,
		0, 0, 1;

	A_hat = MatrixXd(3, 3);
	double tau1 = exp(-deltaT / time_c1);
	double tau2 = exp(-deltaT / time_c2);
	A_hat << 1, 0, 0,
		0, tau1, 0,
		0, 0, tau2;

	B_hat = MatrixXd(3,1);
	B_hat << -deltaT / (3600 * Q_m), 1 - tau1, 1 - tau2;
	
    C_hat = MatrixXd(1, 3);
	C_hat << dOCV_Z, -eff_r1, -eff_r2;

	D_hat = MatrixXd(1, 1);
	D_hat << 1;

	Sigma_Y = VectorXd(1);
	Sigma_Y << 0.05;
	Sigma_X = MatrixXd(3, 3);
	Sigma_X << 0.001, 0, 0,
		0, 0.001, 0,
		0, 0, 0.001;
	x_ = VectorXd(3);
	x_ << soc_m, 0, 0;

}
module::~module()
{

}