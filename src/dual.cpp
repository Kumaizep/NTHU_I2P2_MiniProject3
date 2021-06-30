#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <queue>
#include <vector>

const std::string file_state = "";
const std::string file_action = "";
const int timeout = 1;
void launch_executable(std::string filename) {
	std::string command = filename;
	system(command.c_str());
}

int main(int argc, char** argv)
{
	assert(argc == 3);
	int ans = 0;
	std::string str2, str3, str4, str5;
	if (argv[2][0] == '0') {
		str2 = std::string("./main ../baseline/linux/baseline2 ") + argv[1] + " > tmpout";
		str3 = std::string("./main ../baseline/linux/baseline3 ") + argv[1] + " > tmpout";
		str4 = std::string("./main ../baseline/linux/baseline4 ") + argv[1] + " > tmpout";
		str5 = std::string("./main ../baseline/linux/baseline5 ") + argv[1] + " > tmpout";
	}
	else {
		str2 = std::string("./main ") + argv[1] + " ../baseline/linux/baseline2 > tmpout";
		str3 = std::string("./main ") + argv[1] + " ../baseline/linux/baseline3 > tmpout";
		str4 = std::string("./main ") + argv[1] + " ../baseline/linux/baseline4 > tmpout";
		str5 = std::string("./main ") + argv[1] + " ../baseline/linux/baseline5 > tmpout";
	} 

	std::string logs;

	launch_executable(str2);
	std::ifstream fin("logs/winner.txt");
	if (fin >> logs)
	{
		if (logs[0] == 'O') {
			std::cout << "O ";
		}
		else {
			ans++;
			std::cout << "X ";
		}
	}
	fin.close();
	launch_executable(str3);
	std::ifstream fin2("logs/winner.txt");
	if (fin2 >> logs)
	{
		if (logs[0] == 'O') {
			std::cout << "O ";
		}
		else {
				ans++;
				std::cout << "X ";
			}
		}
		fin2.close();
		launch_executable(str4);
		std::ifstream fin3("logs/winner.txt");
		if (fin3 >> logs)
		{
			if (logs[0] == 'O') {
				std::cout << "O ";
			}
			else {
				ans++;
				std::cout << "X ";
			}
		}
		fin3.close();
		launch_executable(str5);
		std::ifstream fin4("logs/winner.txt");
		if (fin4 >> logs) {
		if (logs[0] == 'O') {
			std::cout << "O\n";
		}
		else {
			ans++;
			std::cout << "X\n";
		}
	}
	fin4.close();
	return 0;
}