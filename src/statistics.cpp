#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

const std::string file_state = "";
const std::string file_action = "";
const int timeout = 1;
void launch_executable(std::string filename) {
	std::string command = filename;
	system(command.c_str());

}

int main(int argc, char const *argv[])
{
	assert(argc == 5);
	int times, count_O = 0, count_X = 0, count_D = 0;
	std::istringstream(argv[4]) >> times;
	std::stringstream ss;
	ss << argv[1] << " " << argv[2] << " " << argv[3] << "> logs/tmpout" << "\n";
	std::string str = ss.str(), logs;
	for (int i = 0; i < times; ++i)
	{
		launch_executable(str);
		std::ifstream fin("logs/tmpout");
		while (fin >> logs)
		{
			if (logs == "Winner")
			{
				fin >> logs;
				fin >> logs;
				if (logs[0] == 'O')
					count_O++;
				else if (logs[0] == 'X')
					count_X++;
				else
					count_D++;
				break;
			}
		}
		std::cout << "Round [" << i + 1 << "/" << times << "] " << logs[0] <<"\n";
		fin.close();
	}
	std::ofstream log("logs/statlog.txt");
	log << argv[2] << ":\n\tO win " << count_O << " times(" << count_O * 100.0 / times << "%).\n";
	log << argv[3] << ":\n\tX win " << count_X << " times(" << count_X * 100.0 / times << "%).\n";
	log << "Draw " << count_D << " time(" << count_D * 100.0 / times << "%).\n";
	log.close();
	return 0;
}