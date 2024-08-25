Battery Management System - BMS

This project is a simple Battery Management System inteded for various applications where a Battery Management might be needed like an xEV or a SmartPhone or any toehr applications
 *** External Libraries Required***
Please visit : https://eigen.tuxfamily.org/index.php?title=Main_Page to get the latest version. 
Steps to include this library in Visual Studio
1. This project was built in accordance with Eigen 3.4.
2. Download the release package.
3. Extract it and save it anywhere on your system.
4. Make sure to add this folder as part of the Include directories under "Project -> Properties -> Configuration Properties -> VC++ Directories "

Typical BMS on any xEV has mainly these functions :
1. Battery Contactor Opening or Closing
2. Battery Hardware protection by observing Temperature, Current and Voltage
3. Battery State-of-Charge (SOC) Estimation
4. Battery Power and Current Limits for Consumers
5. Battery State-of-Health (SOH) Estimation

This project currently focuses on Point 3, 4 and has a basic skeleton for Point 5. The reason being these are the main parts of BMS that require certain algorithms whereas point 1 and point 2 are more 
related to the Hardware and approproiate Battery contactor actuation and communication to the Battery users!

The project has the following components:
1. Class "cell_param" - This class stores the battery cell specifications and some useful functions to retrieve the required data for various functions. The specifications need to be varied based on cell chemistry

Future scope - This class can be modified to incorporate any change in the battery cell specifications as the battery ages and degrades such as the capacity decrease and the resistance increases.
2. Class "module" - A typical battery usually consists of several modules arranged in either series or parallel and each module contains certain number of cells again arranged in series or parallel.
Here, the project assumes that each module has 3 cells arranged in parallel and 12 modules arrange in series to form the battery pack. 
This class has the main state of charge estimation algorithm using Extended Kalman filter. This uses several of the "cell_param" functions and the data.

Future scope - Currently this class approximates the 3 cells in series as a single cell with equivalent resistances and capacitance. This can be improved by running estimations on each individual cell. And this might 
need additional class to handle this. 

3. Namespace "Helper_fn"

This namespace contains the Power and current estimation and State-of-Health estimation function. Currently, this project is esimtating this for the entire battery pack as a whole and that
is why they are grouped here. It also contaions function to write the calculated outputs onto a log file.

4 "BMS.cpp"

This is the main function that contains "module" and "cell_param" objects. It reads input data from the Input file and then calls the necessary functions and outputs the log file by calling the helper functions

Rubrics addressed in this project:

1. Classes and functions, Objected oriented Programming - "module.h", "module.cpp", "cell_param.h", "cell_param.cpp"
2. "BMS.cpp" reads input from a file and outputs the calculations onto a file - Lines - 45, 48
3. Use of references - "Helper_fn.cpp" Line 85,115
4. Use of Shared pointers - "BMS.cpp" - Line 18,27
5. Multi-threading -"BMS.cpp" - Line ,94
6. Mutex and Lock Guard - "Helper_fn.cpp" Line 118
