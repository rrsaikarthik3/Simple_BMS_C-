#include "Helper_fn.h"
#include <iostream>
#include <algorithm>
#include <stdio.h>
#include <vector>
#include <mutex> 
#include <math.h>
#include <fstream>


std::mutex mtx;

/// Power and Current limits estimation for battery pack which can be used by the consumers of the battery!
std::vector<double> Helper_fn::power_est(std::vector<std::shared_ptr<module>> modules, std::shared_ptr<cell_param> cp, double T, double delT, double soc_min, int num_cell_mod, int num_mod)
{
	std::vector<double> limits;
	
	double soc_max, cap, r0;
	soc_max = 0;
	double v_max_m = 3; // initial value to start with
	double v_min_m = 5; //initial value
	for (int i = 0; i < modules.size(); i++)
	{
		double temp = modules[i]->get_volt();

		if (v_min_m > temp)
		{
			v_min_m = temp;
		}
		if (v_max_m < temp)
		{
			v_max_m = temp;
		}
		if (soc_max < modules[i]->get_soc())
		{
			soc_max = modules[i]->get_soc();
		}

	}
	double min_dis_curr, max_chg_curr;
	r0 = cp->get_R0_from_temp(T);

	// Current limits based on voltage limitation
	min_dis_curr = ((v_min_m - cp->get_min_volt()) / r0);
	max_chg_curr = ((v_max_m - cp->get_max_volt()) / r0);

	cap = cp->get_Cap();

	// current limits based on SOC limitation
	min_dis_curr = std::min(min_dis_curr, (soc_min - cp->get_min_soc()) * cap * 3600 / (delT));
	max_chg_curr = std::max(max_chg_curr, (soc_max - cp->get_max_soc()) * cap * 3600 / (delT));

	limits.push_back(min_dis_curr* num_cell_mod); // summing for all cells in module
	limits.push_back(max_chg_curr*num_cell_mod);


	double new_z, new_V;
	///Power Limit calculation
	new_z = soc_min - (min_dis_curr * delT) / (3600 * cap);
	new_V = cp->get_ocv_from_soc(new_z, T) - min_dis_curr * r0;

	limits.push_back(new_V*min_dis_curr * num_cell_mod * num_mod);

	new_z = soc_max + (max_chg_curr * delT) / (3600 * cap);
	new_V = cp->get_ocv_from_soc(new_z, T) - max_chg_curr * r0;

	limits.push_back(new_V*max_chg_curr * num_cell_mod * num_mod);

	return limits;
}

double Helper_fn::avg_soc(std::vector<std::shared_ptr<module>> modules)
{
	// This is a crude estimation of overall Battery SOC for User Interface whereas in reality there is no clear definition of overall Battery SOC as each cell and module can have different
	//SOCs
	double min_soc = 1;
	for (int i = 0; i < modules.size(); i++)
	{
		if (min_soc > modules[i]->get_soc())
			min_soc = modules[i]->get_soc();

	}
	return min_soc;
}
double Helper_fn::Cap_update(std::vector<double> &currents, std::vector<double> soc, double delta_T)
{

	/* This algorithm needs actual data from battery so that this can be tuned along with the SOC estimation! This acts as a placeholder function for now*/
	static double c1, c2 = 0;
	double gamma = 0.998;
	static double cap = 31;
	static double Q;
	double x, y, filt_coeff;
	y = 0;
	filt_coeff = 0.9;
	double sigma_y = 10;
	
	for (int i = 1; i < soc.size(); i++)
	{
		x = soc[i] - soc[0];
		y = y - currents[i-1]* delta_T;
		c1 = gamma*c1 + pow( (x/sigma_y), 2);
		c2 = gamma*c2 + (x * y) / (pow(sigma_y, 2));
	}
	Q = c2 / c1;
	if (Q < cap)
	{
		cap = cap + 0.9 * (Q - cap);  //first order filter
	}
	std::cout << "\n" << Q;
	
	return cap;

}
bool Helper_fn::write_file(const std::string &filename, std::string Header_1, std::string Header_2, std::string Header_3, std::string Header_4, std::string Header_5, std::string Header_6)
{
	/// This function is logging the BMS output on a log file for now, ideally this is communicated to the Supervisory control unit that uses the Battery States and limits for Motor actuations
	std::lock_guard<std::mutex> csvLock(mtx);  // this makes sure that nobody is accessing this file when we are writing, ideally this would be some global variable used by other users to read for actuator command
	std::fstream file;
	file.open(filename, std::ios::out | std::ios::app);
	if (file) {
		file << "\"" << Header_1 << "\",";
		file << "\"" << Header_2 << "\",";
		file << "\"" << Header_3 << "\",";
		file << "\"" << Header_4 << "\",";
		file << "\"" << Header_5 << "\",";
		file << "\"" << Header_6 << "\"";
		file << std::endl;
		return true;
	}
	else {
		return false;
	}
}
