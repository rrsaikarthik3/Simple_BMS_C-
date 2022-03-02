#include "cell_param.h"
#include <iostream>
#include <stdio.h>
cell_param::cell_param()
{
	
	OCV_tbl = MatrixXd(7, 3);

	OCV_tbl << 3.4966, 3.5057, 3.5147, //Z = 0
		3.5519, 3.5660, 3.5653, // z= 0.1
		3.6183, 3.6337, 3.6402, // z = 0.25
		3.7066, 3.7127, 3.7213, // z = 0.5
		3.9131, 3.9259, 3.9376, // z = 0.75
		4.0748, 4.0777, 4.0821, // z= 0.95
		4.1923, 4.1928, 4.1930; // z= 1.0  //Voltage 

	Q0 = 31.0;
	R0 = { 0.0110, 0.0085, 0.0090 }; //Ohms
	R1 = { 0.0065, 0.0025, 0.0012 };   ///Ohms
	C1 = { 11412.82, 23741.14, 34740.428 }; //Farads
	dOCV_dz = {0.6532,0.6433,0.6422}; //V/Ohm

}
/* Helper Functions for cell parameter fetching*/
vector<int> cell_param::get_minmaxind(double x, vector<double> Arr) //getting min max index of the given element in the array
{
	int min_i = 0;
	int mid;
	int max_i = Arr.size() - 1;
	
	if (x < Arr[0])
	{
		x = Arr[0];
	}
	else if (x > Arr[max_i])
	{
		x = Arr[max_i];
	}
	while (max_i - min_i > 1)
	{
		mid = (max_i + min_i) / 2 ;
		if (x > Arr[mid])
		{
			min_i = mid;
		}
		else if(x < Arr[mid])
		{
			max_i = mid;
		}
		else
		{
			min_i = mid;
			max_i = mid + 1;
		}
	}
	if (min_i == max_i)
	{
		max_i = min_i + 1;
	}
	vector<int> temp = { min_i,max_i };
	return(temp);
}
double cell_param::get_x_from_temp(double T, vector<double> p)
{
	vector<int> lim;
	lim = get_minmaxind(T, Temp_brk_pt);
	double m;
	m = (T - Temp_brk_pt[lim[0]]) / (Temp_brk_pt[lim[1]] - Temp_brk_pt[lim[0]]);
	double param = m * p[lim[1]] + (1 - m) * p[lim[0]];
	return param;
}
///************* End of Helper functions ///


double cell_param::get_ocv_from_soc(double z, double T)
{

	int mid;
	vector<double> ocv;
	double v0;
	double m;
	
	vector<int> lim;
	lim = get_minmaxind(z, SOC_brk_pt);

	for (int i = 0; i < Temp_brk_pt.size(); i++)
	{


		m = (z - SOC_brk_pt[lim[0]]) / ( SOC_brk_pt[lim[1]] - SOC_brk_pt[lim[0]] );
		
		
		ocv.push_back(m * OCV_tbl(lim[1], i) + (1-m)* OCV_tbl(lim[0], i));
		
	}

	
	v0 = cell_param::get_x_from_temp(T, ocv);
	return v0;

} 
double cell_param::get_R0_from_temp(double T)
{
	return(cell_param::get_x_from_temp(T, R0));
	
}
double cell_param::get_R1_from_temp(double T)
{
	return(cell_param::get_x_from_temp(T, R1));
}
double cell_param::get_C1_from_temp(double T)
{
	return(cell_param::get_x_from_temp(T, C1));
}
double cell_param::get_dOCV_dz_from_temp(double T)
{
	return(cell_param::get_x_from_temp(T, dOCV_dz));
}
double cell_param::get_Cap()
{
	return Q0;
}
cell_param::~cell_param()
{

}