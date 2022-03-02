#pragma once
#ifndef HELPER_FN_H
#define HELPER_FN_H
#include <memory>
#include "module.h"
#include "cell_param.h"



namespace Helper_fn{
	std::vector<double> power_est(std::vector<std::shared_ptr<module>> modules, std::shared_ptr<cell_param> cp, double T, double delT, double soc_min, int num_cell_mod, int num_mod) ;
	double avg_soc(std::vector<std::shared_ptr<module>> modules);
	double Cap_update(std::vector<double> &currents, std::vector<double> soc, double delta_T);
	bool write_file(const std::string &filename, std::string Header_1, std::string Header_2, std::string Header_3, std::string Header_4, std::string Header_5, std::string Header_6);
};
#endif
