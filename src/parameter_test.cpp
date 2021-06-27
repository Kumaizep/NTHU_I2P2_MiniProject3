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
	int ans;
	std::queue<std::vector<int>> qu;
	std::string str2 = "./main ./player_kumapara ../baseline/linux/baseline2 > tmpout";
	std::string str3 = "./main ./player_kumapara ../baseline/linux/baseline3 > tmpout";
	std::string str4 = "./main ./player_kumapara ../baseline/linux/baseline4 > tmpout";
	std::string str5 = "./main ./player_kumapara ../baseline/linux/baseline5 > tmpout";
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
						std::cout << "Testing " << i << " " << j << " " << k - 5 << " " << h * 3 << " " << g * 10 << "\n";

						ans = 0;
						std::ofstream para("logs/para.txt");
						para << i << " " << j << " " << k - 5 << " " << h * 3 << " " << g * 10 << "\n";
						para.close();
						launch_executable(str2);
						std::ifstream fin("logs/winner.txt");
						if (fin >> logs)
						{
							
							if (logs[0] == 'O') {
								ans++;
								std::cout << "O ";
							}
							else
								std::cout << "X ";
						}
						fin.close();
						launch_executable(str3);
						std::ifstream fin2("logs/gamelog.txt");
						while (fin2 >> logs)
						{
							if (logs == "Winner")
							{
								fin2 >> logs;
								fin2 >> logs;
								if (logs[0] == 'O') {
									ans++;
									std::cout << "O ";
								}
								else
									std::cout << "X ";
								break;
							}
						}
						fin2.close();
						launch_executable(str4);
						std::ifstream fin3("logs/gamelog.txt");
						while (fin3 >> logs)
						{
							if (logs == "Winner")
							{
								fin3 >> logs;
								fin3 >> logs;
								if (logs[0] == 'O') {
									ans++;
									std::cout << "O\n";
								}
								else
									std::cout << "X\n";
								break;
							}
						}
						fin3.close();
						if (ans == 3)
							qu.push({i, j, k - 5, h * 3, g * 10});
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