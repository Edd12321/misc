#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>

#include <sys/signal.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <math.h>
#include <unistd.h>

struct Map {
	std::vector<std::string> grid;
	size_t width = 0, height = 0;
};

float x = -1, y = -1, o;
struct winsize ws;

Map read_map(std::string const& filename)
{
	Map ret_val{};
	std::ifstream fin(filename);
	if (!fin) {
		perror(filename.c_str());
		return ret_val;
	}
	std::string buf;
	while (std::getline(fin, buf)) {
		size_t f;
		if ((f = buf.find('*')) != std::string::npos) {
			if (x == -1 && y == -1) {
				buf[f] = ' ';
				x = f;
				y = ret_val.height;
			} else {
				std::cerr << "map must contain only one *\n";
				return ret_val;
			}
		}
		ret_val.width = std::max(ret_val.width, buf.length());
		ret_val.height++;
		ret_val.grid.push_back(buf);
	}
	if (x == -1 && y == -1) {
		std::cerr << "map must contain * for player spawn\n";
		return ret_val;
	}
	for (auto& it : ret_val.grid)
		if (it.length() < ret_val.width)
			it.insert(it.length(), ret_val.width - it.length(), ' ');
	return ret_val;
}

void play_map(Map const& map)
{
	if (x == -1 && y == -1)
		return;

	constexpr float pi = 3.1415926535;

	auto clamp = [&](float& x, float& y)
	{
		if ((int)x < 0) x = 0;
		if ((int)x >= map.width) x = map.width - 1;
		if ((int)y < 0) y = 0;
		if ((int)y >= map.height) y = map.height - 1;	
	};

	for (;;) {
		float step = pi / ws.ws_col;
		int k = 0;
		std::vector<std::string> buf(ws.ws_row, std::string(ws.ws_col, ' '));
		for (float angle = o + pi / 2; angle >= o - pi / 2; angle -= step) {
			if (k++ >= ws.ws_col)
				break;
			float xx = x, yy = y;
			float sina = sin(angle), cosa = cos(angle);
			while (xx >= 0 && xx < map.width && yy >= 0 && yy < map.height && isspace(map.grid[yy][xx]))
				xx += 0.1 * cosa, yy += 0.1 * sina;
			float dist = sqrt(pow(xx - x, 2) + pow(yy - y, 2));
			int wall_height = ws.ws_row;
			if (dist >= 1)
				wall_height /= dist;
			clamp(xx, yy);
			char texture = map.grid[yy][xx];
			for (int i = (ws.ws_row - wall_height) / 2; i < (ws.ws_row + wall_height) / 2; ++i)
				buf[i][k] = texture;
		}
		for (int i = 0; i < ws.ws_row; ++i)
			std::cout << buf[i] << '\n';
		std::cout << "\033[0;0H";

		char c = getchar();
		float new_x, new_y, mult = 0.2;
		switch (c) {
			case 'q': return;
			case 's':
				mult *= -1; // fallthrough
			case 'w':
				new_x = x+mult*cos(o);
				new_y = y+mult*sin(o);
				if (isspace(map.grid[int(new_y)][int(new_x)]))
					x = new_x, y = new_y;
				clamp(x, y);
				break;
			case 'd':
				mult *= -1; // fallthrough
			case 'a':
				o += mult; o = fmod(o, 2*pi); break;
		}
	}
}

void get_ws(int sig = 0)
{
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
}

int main(int argc, char *argv[])
{
	if (argc < 2) {
		std::cerr << "usage: " << argv[0] << " <map>\n";
		return 1;
	}

	get_ws();
	signal(SIGWINCH, get_ws);

	struct termios oldt, newt;
	if (tcgetattr(STDIN_FILENO, &oldt) == -1) {
		perror("tcgetattr");
		return 1;
	}
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &newt);
	std::cout << "\033[?25l";
	play_map(read_map(argv[1]));
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &oldt);
	std::cout << "\033[2J\033[?25h";
	return 0;
}
