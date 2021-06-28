#include <array>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <unordered_set>
#include <vector>
#include <limits>
#include <cmath>

#define SIZE 8
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

struct Node {
	int nplayer;
	double win = 0;
	int simu = 0;
	bool is_leaf = true;
	Point ndir = Point(-1, -1);
	Node* parent = NULL;
	std::array<std::array<int, SIZE>, SIZE> nboard;
	std::unordered_set<Node*> nchild;
	// Node(int np, std::array<std::array<int, SIZE>, SIZE> nb) : nplayer(np), nboard(nb) {}
};


// ================================================================================================
// Global variable
// ================================================================================================
int player;
int cur_player;
int board_count = 0;
int mctscount = 0;
const int BASELINE1 = 1, BASELINE2 = SIZE - 1;
const int EMPTY = 0, BLACK = 1, WHITE = 2;
// const int PARA[5] = {17, 1, 0, 0, 10}; 
const int PARA[5] = {40, 2, -5, 15, 70}; 
	// Depth, normal, backline, boundary, arc  
std::clock_t start_time;
const std::clock_t TIMEOUT = 9850000;
std::array<std::array<int, SIZE>, SIZE> board;
std::array<std::array<int, SIZE>, SIZE> cur_board;
std::vector<Point> next_valid_spots;
const std::array<Point, 8> directions{{
	Point(-1, -1), Point(-1, 0), Point(-1, 1), Point(0, -1),
	Point(0, 1), Point(1, -1), Point(1, 0), Point(1, 1)}};
// const int weight[SIZE][SIZE] = {{ 90, -60, 20, 10, 10, 20, -60,  90},
// 								{-60, -80, 10,  5,  5, 10, -80, -60},
// 								{ 20,  10,  5,  1,  1,  5,  10,  20},
// 								{ 10,   5,  1,  1,  1,  1,   5,  10},
// 								{ 10,   5,  1,  1,  1,  1,   5,  10},
// 								{ 20,  10,  5,  1,  1,  5,  10,  20},
// 								{-60, -80, 10,  5,  5, 10, -80, -60},
// 								{ 90, -60, 20, 10, 10, 20, -60,  90}};

// ================================================================================================
// Function/Board operater
// ================================================================================================
void debug_out_board(Node* root) {
	std::cout << "===========================\n";
	for (int i = 0; i < SIZE; ++i) {
		for (int j = 0; j < SIZE; ++j) {
			std::cout << root->nboard[i][j] << " ";
		}
		std::cout << "\n";
	}
	std::cout << "===========================\n"; 
}

int get_next_player(int pl) { return 3 - pl; }

bool is_spot_on_board(Point p) { return 0 <= p.x && p.x < SIZE && 0 <= p.y && p.y < SIZE; }

int get_disc(Point p, Node* root) { return root->nboard[p.x][p.y]; }

void set_disc(Point p, int disc, Node* root) { root->nboard[p.x][p.y] = disc; }

bool is_disc_at(Point p, int disc, Node* root) {
	if (!is_spot_on_board(p))
		return false;
	if (get_disc(p, root) != disc)
		return false;
	return true;
}

bool is_spot_valid(Point center, Node* root) {
	if (get_disc(center, root) != EMPTY) {
		// std::cout << " >> nonempty!\n";
		return false;
	}
	// std::cout << "Vaild Check " << root->nplayer <<"\n";
	// debug_out_board(root);
	for (Point dir : directions) {
		// Move along the direction while testing.
		// std::cout << " >> check dir (" << dir.x << ", " << dir.y << ")\n";
		Point p = center + dir;
		if (!is_disc_at(p, get_next_player(root->nplayer), root))
			continue;
		p = p + dir;
		while (is_spot_on_board(p) && get_disc(p, root) != EMPTY) {
			if (is_disc_at(p, root->nplayer, root)) {
				// std::cout << "Vaild: (" << center.x << ", " << center.y << ")\n";
				return true;
			}
			p = p + dir;
		}
	}
	// std::cout << " >> (" << center.x << ", " << center.y << ") no nigib! player = " << root->nplayer << "\n";
	return false;
}

std::vector<Point> get_root_valid_spots(Node* root) {
	std::vector<Point> valid_spots;
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			Point p = Point(i, j);
			if (root->nboard[i][j] != EMPTY)
				continue;
			if (is_spot_valid(p, root))
				valid_spots.push_back(p);
		}
	}
	return valid_spots;
}

void flip_discs(Node* root, Point center) {
	for (Point dir : directions) {
		// Move along the direction while testing.
		Point p = center + dir;
		if (!is_disc_at(p, get_next_player(root->nplayer), root))
			continue;
		std::vector<Point> discs({p});
		p = p + dir;
		while (is_spot_on_board(p) && get_disc(p, root) != EMPTY) {
			if (is_disc_at(p, root->nplayer, root)) {
				for (Point s : discs) {
					set_disc(s, root->nplayer, root);
				}
				break;
			}
			discs.push_back(p);
			p = p + dir;
		}
	}
}

bool put_disc(Point p, Node* root) {
	// std::cout << " >> put_disc: start ";
	// debug_out_board(root);
	if (!is_spot_valid(p, root)) {
		// std::cout << " >> invaild?! at(" << p.x << ", " << p.y << ")\n";
		return false;
	}
	set_disc(p, root->nplayer, root);
	flip_discs(root, p);
	// std::cout << " >> put_disc: put ";
	// debug_out_board(root);
	root->nplayer = get_next_player(root->nplayer);
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

int get_node_disc_count(Node* node, int state) {
	int val = 0;
	for (int i = 0; i < SIZE; ++i) {
		for (int j = 0; j < SIZE; ++j) {
			if (node->nboard[i][j] == state)
				val++;
		}
	}
	return val;
}

// ================================================================================================
// Function/Decision
// ================================================================================================

double UCT_func(double w, int n, int t) {
	return w / (n + 0.00000001) + sqrt(2.0 * log(t) / (n + 0.00000001));
}

Node* Selection(Node* root) {
	// std::cout << " >> Selection: start\n";
	// debug_out_board(root);
	Node* tmp = root;
	while (!tmp->is_leaf) {
		// std::cout << " >> Selection: go to lf\n";
		double max = -9999.9;
		Node* next = NULL;
		if (tmp->nchild.empty())
			return tmp;
		for (auto it : tmp->nchild) {
			double val = UCT_func(it->win, it->simu, tmp->simu);
			// std::cout << " >> Selection: visit with " << val << "\n";
			// debug_out_board(it);
			if (val > max) {
				max = val;
				next = it;
			}
		}
		// std::cout << " >> Selection: selected\n";
		// debug_out_board(tmp);
		tmp = next;
	}
	// std::cout << " >> Selection: fin \n";
	// debug_out_board(tmp);
	return tmp;
}

Node* Expansion(Node* root) {
	Node* tmp;
	// debug_out_board(root);
	std::vector<Point> valid_spots = get_root_valid_spots(root);
	// std::cout << "debug 0 " << valid_spots.size() << "\n";
	if (valid_spots.size() == 0)
		return root;
	// std::cout << "debug 0 " << valid_spots.size() << "\n";
	for (auto it : valid_spots)
	{
		// std::cout << "debug 1\n";
		tmp = new Node();
		tmp->nplayer = root->nplayer;
		// std::cout << "debug 2\n";
		tmp->nboard = root->nboard;
		put_disc(it, tmp);
		tmp->parent = root;
		// std::cout << "debug 3\n";
		tmp->ndir = it;
		root->nchild.insert(tmp);
		// std::cout << " >> Expansion: new board\n";
		// debug_out_board(tmp);
	}
	root->is_leaf = false;
	return tmp;
}

int Simulation(Node* root)
{
	// std::cout << " >> Simulation\n";
	// debug_out_board(root);
	// int count = 0;
	int winner = -1;
	Node* tmp = new Node();
	tmp->nboard = root->nboard;
	tmp->nplayer = root->nplayer;
	while (true) {
		// std::cout << " >> Simulation: loop " << count++ << "\n";
		// debug_out_board(tmp);
		std::vector<Point> valid_spots = get_root_valid_spots(tmp);
		if (valid_spots.size() == 0) {
			root->nplayer = get_next_player(root->nplayer);
			valid_spots = get_root_valid_spots(tmp);
			if (valid_spots.size() == 0) {
				// Game ends
				int white_discs = get_node_disc_count(tmp, WHITE);
				int black_discs = get_node_disc_count(tmp, BLACK);
				if (white_discs == black_discs)
					winner = EMPTY;
				else if (black_discs > white_discs)
					winner = BLACK;
				else
					winner = WHITE;
				break;
			}
		}
		else {
			int n_valid_spots = valid_spots.size();
			srand(time(NULL));	
			Point p = valid_spots[(rand() % n_valid_spots)];
			put_disc(p, tmp);
		}
	}
	// std::cout << " >> Simulation: fin " << winner << "\n";
	return winner;
}

void Backpropagation(Node* root, double result)
{
	// std::cout << " >> Backpropagation\n";
	// debug_out_board(root);
	if (root == NULL)
		return;
	root->win += result;
	root->simu++;
	if (root->parent != NULL)
		Backpropagation(root->parent, result);
	// std::cout << " >> Backpropagation: fin\n";
}

Point MCTS(Node* root) {
	while (std::clock() - start_time < TIMEOUT) {
		// std::cout << " >> MCTS round " << round << "\n";
		Node* step1 = Selection(root);
		// std::cout << " >> MCTS Step1 in " << round << "\n";
		// debug_out_board(step1);
		Node* step2 = Expansion(step1);
		// std::cout << " >> MCTS Step2 in " << round << "\n";
		// debug_out_board(step2);
		int simu_result = Simulation(step2);
		if (simu_result == EMPTY)
			Backpropagation(step2, 0.5);
		else if (simu_result == player)
			Backpropagation(step2, 1);
		else
			Backpropagation(step2, 0);
		mctscount++;
	}
	double max = -999;
	Point mdir = Point(-1, -1);
	if (root->nchild.empty())
		return Point(-2, -2);
	for (auto it : root->nchild) {
		double val = it->win / it->simu;
		if (val > max) {
			max = val;
			mdir = it->ndir;
		}
	}
	return mdir;
}

// ================================================================================================
// Function/Player action 
// ================================================================================================
void read_board(std::ifstream& fin) {
	fin >> player;
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			fin >> board[i][j];
			if (board[i][j] != 0)
				board_count++;
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
	Node* root = new Node();
	root->nboard = board;
	root->nplayer = player;
	std::cout << "Start... MCTS" << std::endl;
	Point p = MCTS(root);
	std::cout << "choice " << p.x << " " << p.y << " in " << std::clock() - start_time << " Count: " << mctscount << std::endl;
	fout << p.x << " " << p.y << std::endl;
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
