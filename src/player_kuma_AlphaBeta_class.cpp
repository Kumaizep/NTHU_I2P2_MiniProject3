#include <array>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <vector>
#include <limits>
#include <cmath>

#define SIZE 8
using Board = std::array<std::array<int, SIZE>, SIZE>;
enum SPOT_STATE { EMPTY = 0, BLACK = 1, WHITE = 2 };
enum BASELINE { BASELINE1 = 1, BASELINE2 = SIZE - 1 };

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
int borad_step = 0;
Board board;
std::vector<Point> next_valid_spots;
const int PARA[2][6] = {
	{11, 4, -30, 0, 60, 60},
	{11, 6, -10, 0, 90, 50}}; 
const int PARA_ENDGAME[2][6] = {
	{11, 4, -30, -10, 60, 60},
	{11, 6, -4, 0, 90, 50}}; 
// Parameters: Depth, normal, backline, boundary, arc, stable
const std::array<Point, 8> directions{{
		Point(-1, -1), Point(-1, 0), Point(-1, 1), Point(0, -1),
		Point(0, 1), Point(1, -1), Point(1, 0), Point(1, 1)}};

// ================================================================================================
// Class Gameboard
// ================================================================================================
class GameBorad {
public:
	int cur_player;
	Board cur_board;	

private:
// ================================================================================================
// Function/Board operate
// ================================================================================================
	int get_next_player(int pl) { return 3 - pl; }
	bool is_spot_on_board(Point p) { 
		return 0 <= p.x && p.x < SIZE && 0 <= p.y && p.y < SIZE; }
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
		if (get_disc(center) != EMPTY)
			return false;
		for (Point dir : directions) {
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
			return false;
		}
		set_disc(p, cur_player);
		flip_discs(p);
		cur_player = get_next_player(cur_player);
		return true;
	}
	bool is_terminal() {
		std::vector<Point> valid_spots = get_valid_spots();
		if (valid_spots.size() == 0) {
			cur_player = get_next_player(cur_player);
			valid_spots = get_valid_spots();
			if (valid_spots.size() == 0) {
				return true;
			}
			cur_player = get_next_player(cur_player);
		}
		return false;
	}
	bool check_stable(Point center, Point dir, int state) {
		Point p = center + dir;
		while (is_spot_on_board(p)) {
			if (get_disc(p) != state) {
				return false;
			}
			p = p + dir;
		}
		return true;
	}
	bool is_stable_point(Point center, int state) {
		for (auto it : directions) {
			if (!check_stable(center, it, state) && 
				!check_stable(center, Point(0,0) - it, state)) {
				return false;
			}
		}
		return true;
	}
	int get_disc_count_compensate(int state) {
		int val = 0;
		for (int i = 0; i < SIZE; ++i) {
			for (int j = 0; j < SIZE; ++j) {
				if (cur_board[i][j] == state) {
					val += get_para(1);
					if (i == 0 || j == 0 || i == SIZE - 1 || j == SIZE - 1)
						val += get_para(2);					
					if (i == BASELINE1 || j == BASELINE1 ||
						 i == BASELINE2 || j == BASELINE2)
						val += get_para(3);
					if ((i == 0 && j == 0) || (i == SIZE - 1 && j == SIZE - 1) || 
						(i == SIZE - 1 && j == 0) || (i == 0 && j == SIZE - 1))
						val += get_para(4);				
					if (is_stable_point(Point(i, j), state))
						val += get_para(5);
				}
			}
		}
		return val;
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
	int get_para(int idx) {
		if (borad_step > 50)
			return PARA_ENDGAME[player - 1][idx];
		else
			return PARA[player - 1][idx];
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
	void Set_board(Board nb, int np) {
		cur_board = nb;
		cur_player = np;
	}

// ================================================================================================
// Function/Decision
// ================================================================================================
	int reval(int val) { return val * 2 - 3; }

	int valize(int val) { return val * reval(player); }

	int heuristic() {
		int white_discs = get_disc_count_compensate(WHITE);
		int black_discs = get_disc_count_compensate(BLACK);
		int val = valize(white_discs - black_discs);
		return val;
	}

	int AlphaBeta(Point p, int depth, int alpha, int beta, bool round) {
		int tmp_player = cur_player, best;
		std::vector<Point> cur_valid_spots = get_valid_spots();
		bool vaild = put_disc(p);
		Board tmp_board = cur_board;
		if (!vaild) {
			best = heuristic();
		}
		else if (depth == 0) {
			best = heuristic();
		}
		else if (is_terminal()) {
			best = heuristic() * pow(2, depth);
		}
		else if (round) {
			for (auto it : cur_valid_spots) {
				int val = AlphaBeta(it, depth - 1, alpha, beta, false);
				if (alpha < val)
					alpha = val;
				if (alpha >= beta)
					break;
			}
			best = alpha;
		}
		else {
			for (auto it : cur_valid_spots) {
				int val = AlphaBeta(it, depth - 1, alpha, beta, true);
				if (beta > val)
					beta = val;
				if (alpha >= beta)
					break;
			}
			best = beta;
		}
		cur_player = tmp_player;
		cur_board = tmp_board;
		return best;
	}

public:
	GameBorad(Board cb, int cp) : cur_player(cp), cur_board(cb) {};
	Point El_Psy_Kongroo() {
		int best = std::numeric_limits<int>::min(), val;
		Point p;
		std::vector<Point> valid_spots = get_valid_spots();
		for (auto it : valid_spots) {
			Set_board(board, player);
			val = AlphaBeta(it, get_para(0), best, std::numeric_limits<int>::max(), false);
			if (val > best) {
				best = val;
				p = it;
			}
		}
		return p;
	}
};



// ================================================================================================
// Function/Decision
// ================================================================================================


// ================================================================================================
// Function/Player action 
// ================================================================================================
void read_board(std::ifstream& fin) {
	fin >> player;
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			fin >> board[i][j];
			if (board[i][j] != EMPTY)
				borad_step++;
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
	Point p;
	std::ofstream dlout("logs/decision.txt");
	GameBorad game(board, player);
	p = game.El_Psy_Kongroo();
	std::cout << "choice " << p.x << " " << p.y << std::endl;
	fout << p.x << " " << p.y << std::endl;
	dlout.close();
	fout.flush();
}

// ================================================================================================
// Main
// ================================================================================================
int main(int, char** argv) {
	std::ifstream fin(argv[1]);
	std::ofstream fout(argv[2]);
	read_board(fin);
	read_valid_spots(fin);
	write_valid_spot(fout);
	fin.close();
	fout.close();
	return 0;
}
