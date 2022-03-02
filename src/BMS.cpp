#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <chrono>
#include "time.h"
#include <vector>
#include <thread>
#include "cell_param.h"
#include "module.h"
#include <memory>
#include "Helper_fn.h"



int main()
{
	std::shared_ptr<cell_param> cp = std::make_shared<cell_param>();
	/*Battery pack arrangement*/
	module_type mod_typ = module_type::parallel;
	double rated_volt_bat = 48;  

	int num_cell_mod = 3;  //number of cells in module
	int num_mod = 12;   //Number of modules - 48 V Battery pack with 3240Ah capacity total
	double init_Temp = 25;
	double init_soc = 0.9;
	std::vector<std::shared_ptr<module>> modules;
	std::vector<std::thread> module_threads;
	std::vector<double> acc_current, acc_sc;
	double updated_cap = cp->get_Cap();
	double soh = 1.0; //State pf health initial is assumed to be 100%
	double st_time_wind, lt_time_wind;
	double init_volt = cp->get_ocv_from_soc(init_soc, init_Temp);
	st_time_wind = 1;
	lt_time_wind = 5;

	for (int i = 0; i < num_mod; i++)
	{
		modules.emplace_back(new module(init_volt, 0, init_soc,i+1));
		modules[i]->update_cell_param(init_Temp, cp, num_cell_mod, mod_typ);
	}

	int f_i = 0;
	
	std::ifstream filestream("Data/Curr_Volt_Temp_Data.csv");
	std::string line;

	std::string Out_LOG = "Data/Output_log.csv";
	std::ofstream myfile;
	myfile.open(Out_LOG); 
	myfile << "\nDischarge Power Limits (kW), ST - Discharge Power Limits (kW), ST - Discharge Current Limits (A), ST - Charge Current Limits (A), Battery SOC %, Battery SOH % \n";
	myfile.close();
	
	
	std::chrono::time_point<std::chrono::system_clock> prev = std::chrono::system_clock::now();std::chrono::steady_clock::now();

	if (filestream.is_open())
	{
		while (std::getline(filestream, line))
		{
			if (f_i == 0)
			{
				f_i++;
				continue;
			}
			f_i++;
			double volt;
			double current;
			double Temp;
			std::replace(line.begin(), line.end(), ',', ' ');
			std::istringstream linestream(line);
			linestream >> current;
			linestream >> Temp;
			acc_current.push_back(current);
			for (int j = 0; j < num_mod; j++)
			{
				linestream >> volt;
				modules[j]->set_volt(volt);
				modules[j]->set_curr(current);
				modules[j]->update_cell_param(Temp, cp, num_cell_mod, mod_typ);

				/* In this algorithm, we are appying Extended Kalman Filter for each module and are approximating each module as a single cell by approximating the resostances and capacitances,
				In a real life scenario, each module would have access to each and every cell's data and estimations through which the EKF is applied to each and every cell individually. Based on applications,
				each module has its own dedicated micro controller. in order to mimic this, each module step is run on different threads where the results of each calculation can come at any random order to 
				the central BMS controller */
				module_threads.emplace_back(std::thread(&module::ekf_soc_predict,modules[j])); //prediction step
			}
			for (auto& t : module_threads)
				t.join();
			module_threads.clear();
			for (int j = 0; j < num_mod; j++)
			{
				modules[j]->update_cell_param(Temp, cp, num_cell_mod, mod_typ);
				module_threads.emplace_back(std::thread(&module::ekf_soc_update, modules[j]));  //Update step after prediction step
			}
			
			for (auto& t : module_threads)
				t.join();
			module_threads.clear();

			double SOC_Bat = Helper_fn::avg_soc(modules);
			acc_sc.push_back(SOC_Bat);

			vector<double> limits = Helper_fn::power_est(modules, cp, Temp, st_time_wind, SOC_Bat, num_cell_mod,num_mod);

			std::this_thread::sleep_for(std::chrono::milliseconds(100)); //mimic actual sensor reasing

			auto elapsed = std::chrono::system_clock::now() - prev;
			if (elapsed.count() >= 100000000) //Capacity prediction is spaced out to have minimum of 50 data points to estimate, currently this algorithm needs actual battery data for proper tuning!
			{
				std::cout << "\n" << acc_current.size();
				updated_cap = Helper_fn::Cap_update(acc_current, acc_sc,0.1);

				soh = updated_cap / cp->get_Cap();
				acc_current.clear();
				acc_sc.clear();
				prev = std::chrono::system_clock::now();
			}
			if (!Helper_fn::write_file(Out_LOG, std::to_string(limits[2] / 1000), std::to_string(limits[3] / 1000), std::to_string(limits[0]), std::to_string(limits[1]), std::to_string(SOC_Bat * 100), std::to_string(soh * 100)))
			{
				std::cout << "\n Some error in SW communication!";  
			}
		}
	}
	return 0;

}