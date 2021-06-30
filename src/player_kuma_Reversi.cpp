#include <array>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <vector>
#include <limits>


// ================================================================================================
// Struct
// ================================================================================================
struct Point {
	int x, y;
	Point() : Point(0, 0) {}
	Point(int x, int y) : x(x), y(y) {}
	bool operator==(const Point& rhs) const { return x == rhs.x && y == rhs.y; }
	bool operator!=(const Point& rhs) const { return !operator==(rhs); }
	Point operator+(const Point& rhs) const {
		return Point(x + rhs.x, y + rhs.y);
	}
	Point operator-(const Point& rhs) const {
		return Point(x - rhs.x, y - rhs.y);
	}
};


// ================================================================================================
// Global variable
// ================================================================================================
int player;
int cur_player;
const int SIZE = 8;
const int BASELINE1 = 1, BASELINE2 = SIZE - 1;
const int EMPTY = 0, BLACK = 1, WHITE = 2;
// const int PARA[5] = {17, 1, 0, 0, 10}; 
const int PARA[5] = {10, 2, -5, 15, 70}; 
	// Depth, normal, backline, boundary, arc  
std::clock_t start_time;
std::array<std::array<int, SIZE>, SIZE> board;
std::array<std::array<int, SIZE>, SIZE> cur_board;
std::vector<Point> next_valid_spots;
const std::array<Point, 8> directions{{
	Point(-1, -1), Point(-1, 0), Point(-1, 1), Point(0, -1),
	Point(0, 1), Point(1, -1), Point(1, 0), Point(1, 1)}};
const int weight2[SIZE][SIZE] = {
	{99,  -8,  8,  6,  6,  8,  -8, 99},
	{-8, -24, -4, -3, -3, -4, -24, -8},
	{ 8,  -4,  7,  4,  4,  7,  -4,  8},
	{ 6,  -3,  4,  0,  0,  4,  -3,  6},
	{ 6,  -3,  4,  0,  0,  4,  -3,  6},
	{ 8,  -4,  7,  4,  4,  7,  -4,  8},
	{-8, -24, -4, -3, -3, -4, -24, -8},
	{99,  -8,  8,  6,  6,  8,  -8, 99}};
const int weight[SIZE][SIZE] = {
	{20, -3, 11,  8,  8, 11, -3, 20},
	{-3, -7, -4,  1,  1, -4, -7, -3},
	{11, -4,  2,  2,  2,  2, -4, 11},
	{ 8,  1,  2, -3, -3,  2,  1,  8},
	{ 8,  1,  2, -3, -3,  2,  1,  8},
	{11, -4,  2,  2,  2,  2, -4, 11},
	{-3, -7, -4,  1,  1, -4, -7, -3},
	{20, -3, 11,  8,  8, 11, -3, 20}};


// ================================================================================================
// Function/Board operater
// ================================================================================================
int get_next_player(int pl) { return 3 - pl; }

bool is_spot_on_board(Point p) { return 0 <= p.x && p.x < SIZE && 0 <= p.y && p.y < SIZE; }

int get_disc(Point p) { return cur_board[p.x][p.y]; }

void set_disc(Point p, int disc) { cur_board[p.x][p.y] = disc; }

bool is_disc_at(Point p, int disc) {
	if (!is_spot_on_board(p))
		return false;
	if (get_disc(p) != disc)
		return false;
	return true;
}

bool is_spot_valid(Point center) {
	if (get_disc(center) != EMPTY) {
		// std::cout << " >> nonempty!\n";
		return false;
	}
	for (Point dir : directions) {
		// Move along the direction while testing.
		// std::cout << " >> check dir (" << dir.x << ", " << dir.y << ")\n";
		Point p = center + dir;
		if (!is_disc_at(p, get_next_player(cur_player)))
			continue;
		p = p + dir;
		while (is_spot_on_board(p) && get_disc(p) != EMPTY) {
			if (is_disc_at(p, cur_player))
				return true;
			p = p + dir;
		}
	}
	// std::cout << " >> (" << center.x << ", " << center.y << ") no nigib! cur_player = " << cur_player << "\n";
	return false;
}

std::vector<Point> get_valid_spots() {
	std::vector<Point> valid_spots;
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			Point p = Point(i, j);
			if (cur_board[i][j] != EMPTY)
				continue;
			if (is_spot_valid(p))
				valid_spots.push_back(p);
		}
	}
	return valid_spots;
}

void flip_discs(Point center) {
	for (Point dir : directions) {
		// Move along the direction while testing.
		Point p = center + dir;
		if (!is_disc_at(p, get_next_player(cur_player)))
			continue;
		std::vector<Point> discs({p});
		p = p + dir;
		while (is_spot_on_board(p) && get_disc(p) != EMPTY) {
			if (is_disc_at(p, cur_player)) {
				for (Point s : discs) {
					set_disc(s, cur_player);
				}
				break;
			}
			discs.push_back(p);
			p = p + dir;
		}
	}
}

bool put_disc(Point p) {
	if (!is_spot_valid(p)) {
		// std::cout << " >> invaild?! at(" << p.x << ", " << p.y << ")\n";
		return false;
	}
	set_disc(p, cur_player);
	flip_discs(p);
	// Give control to the other player.
	cur_player = get_next_player(cur_player);
	return true;
}

int get_disc_count(int state) {
	int val = 0;
	for (int i = 0; i < SIZE; ++i) {
		for (int j = 0; j < SIZE; ++j) {
			if (cur_board[i][j] == state)
				val++;
		}
	}
	return val;
}

int get_disc_count_compensate(int state) {
	int val = 0;
	for (int i = 0; i < SIZE; ++i) {
		for (int j = 0; j < SIZE; ++j) {
			if (cur_board[i][j] == state)
			{
				val += weight[i][j];
				// val += PARA[1];

				// if (i == 0 || j == 0 || i == SIZE - 1 || j == SIZE - 1)
				// 	val += PARA[2];
					
				// if (i == BASELINE1 || j == BASELINE1 || i == BASELINE2 || j == BASELINE2)
				// 	val += PARA[3];

				// if ((i == 0 && j == 0) || (i == SIZE - 1 && j == SIZE - 1) || 
				// 	(i == SIZE - 1 && j == 0) || (i == 0 && j == SIZE - 1))
				// 	val += PARA[4];				
			}
		}
	}
	return val;
}

void debug_out_board() {
	std::cout << "===========================\n";
	for (int i = 0; i < SIZE; ++i) {
		for (int j = 0; j < SIZE; ++j) {
			std::cout << cur_board[i][j] << " ";
		}
		std::cout << "\n";
	}
	std::cout << "===========================\n"; 
}

// ================================================================================================
// Function/Player action 
// ================================================================================================
void read_board(std::ifstream& fin) {
	fin >> player;
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			fin >> board[i][j];
		}
	}
}

void read_valid_spots(std::ifstream& fin) {
	int n_valid_spots;
	fin >> n_valid_spots;
	int x, y;
	for (int i = 0; i < n_valid_spots; i++) {
		fin >> x >> y;
		next_valid_spots.push_back({x, y});
	}
}

void write_valid_spot(std::ofstream& fout) {
	srand(time(NULL));
	// Choose random spot. (Not random uniform here)
	int best = std::numeric_limits<int>::min();
	Point p;
	// std::cout << player << " is our player.\n";
	// std::ofstream dlout("logs/decision.txt");
	// dlout << next_valid_spots.size() << " vailds.\n";
	for (auto it : next_valid_spots) {
		int val = weight[it.x][it.y];
		// dlout << " >> mmh_val of " << "(" << it.x << ", " << it.y << ")" << ": " << val << "\n";
		if (val > best) {
			best = val;
			p = it;
		}
	}
	// Remember to flush the output to ensure the last action is written to
	// file.
	std::cout << "choice " << p.x << " " << p.y << " in " << std::time(nullptr) << std::endl;
	fout << p.x << " " << p.y << std::endl;
	// dlout.close();
	fout.flush();
}

// ================================================================================================
// Main
// ================================================================================================
int main(int, char** argv) {
	start_time = std::clock();
	std::ifstream fin(argv[1]);
	std::ofstream fout(argv[2]);
	read_board(fin);
	read_valid_spots(fin);
	write_valid_spot(fout);
	fin.close();
	fout.close();
	return 0;
}
