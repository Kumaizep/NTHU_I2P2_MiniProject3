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

int main()
{
	int ans = 0;
	std::queue<std::vector<int>> qu;
	std::string str2 = "./main ../baseline/linux/baseline2 ./player_kuma_ABpara > tmpout";
	std::string str3 = "./main ../baseline/linux/baseline3 ./player_kuma_ABpara > tmpout";
	std::string str4 = "./main ../baseline/linux/baseline4 ./player_kuma_ABpara > tmpout";
	std::string str5 = "./main ../baseline/linux/baseline5 ./player_kuma_ABpara > tmpout";
	std::string logs;
	for (int i = 10; i < 20; ++i)
	{
		for (int j = 0; j < 10; ++j)
		{
			for (int k = 0; k < 10; ++k)
			{
				for (int h = 0; h < 10; ++h)
				{
					for (int g = 0; g < 10; ++g)
					{
						std::cout << "Testing " << i << " " << j << " " << k - 5 << " " << h * 3 << " " << g * 10 << "\t\t";

						ans = 0;
						std::ofstream para("logs/para.txt");
						para << i << " " << j << " " << k - 5 << " " << h * 3 << " " << g * 10 << "\n";
						para.close();
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
						if (ans == 4) {
							std::ofstream log("logs/last4.txt");
							log << i << " " << j << " " << k - 5 << " " << h * 3 << " " << g * 10 << "\n";
							log.close();
							qu.push({i, j, k - 5, h * 3, g * 10});
							ans = 0;
						} 
						else if (ans == 3) {
							std::ofstream log("logs/last3.txt");
							log << i << " " << j << " " << k - 5 << " " << h * 3 << " " << g * 10 << "\n";
							log.close();
							ans = 0;
						}
					}
				}
			}
		}
	}
	std::ofstream log("logs/statlog.txt");
	while (!qu.empty())
	{
		std::vector<int> apara = qu.front();
		log << apara[0] << " " << apara[1] << " " << apara[2] << " " << apara[3] << " " << apara[4] << "\n";
		qu.pop();
	}
	log.close();
	return 0;
}