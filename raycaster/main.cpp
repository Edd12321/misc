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
	std::vector<std::wstring> grid;
	size_t width = 0, height = 0;
};

double x = -1, y = -1, o;
struct winsize ws;

Map read_map(std::string const& filename)
{
	Map ret_val{};
	std::wifstream fin(filename);
	if (!fin) {
		perror(filename.c_str());
		return ret_val;
	}
	std::wstring buf;
	while (std::getline(fin, buf)) {
		size_t f;
		if ((f = buf.find(L'*')) != std::wstring::npos) {
			if (x == -1 && y == -1) {
				buf[f] = ' ';
				x = f;
				y = ret_val.height;
			} else {
				std::wcerr << L"map must contain only one *\n";
				return ret_val;
			}
		}
		ret_val.width = std::max(ret_val.width, buf.length());
		ret_val.height++;
		ret_val.grid.push_back(buf);
	}
	if (x == -1 && y == -1) {
		std::wcerr << L"map must contain * for player spawn\n";
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

	bool t = false, g = true, f = false; // not textured, show lighting

	auto clamp = [&](double& x, double& y)
	{
		if ((int)x < 0) x = 0;
		if ((int)x >= map.width) x = map.width - 1;
		if ((int)y < 0) y = 0;
		if ((int)y >= map.height) y = map.height - 1;	
	};

	for (;;) {
		double step = M_PI/2 / ws.ws_col;
		int k = 0;
		std::vector<std::wstring> buf(ws.ws_row, std::wstring(ws.ws_col, ' '));

		// Floor
		if (g)
			for (size_t i = ws.ws_row / 2 + 1; i < ws.ws_row; ++i)
				buf[i] = std::wstring(ws.ws_col, ',');

		for (double angle = o + M_PI / 4; k < ws.ws_col && angle >= o - M_PI / 4; angle -= step, ++k) {
			double xx = x, yy = y;
			double sina = sin(angle), cosa = cos(angle);
			while (xx >= 0 && xx < map.width && yy >= 0 && yy < map.height && iswspace(map.grid[yy][xx]))
				xx += 0.1 * cosa, yy += 0.1 * sina;
			double dist = sqrt(pow(xx - x, 2) + pow(yy - y, 2)) * (f ? 1 : cos(angle - o));
			int wall_height = ws.ws_row;
			if (dist >= 1)
				wall_height /= dist;
			clamp(xx, yy);
			wchar_t texture = t ? map.grid[yy][xx] : L"█▓▒░:. "[std::min(int(dist / 6), 6)];
			for (int i = (ws.ws_row - wall_height) / 2; i < (ws.ws_row + wall_height) / 2; ++i)
				buf[i][k] = texture;
		}
		for (int i = 0; i < ws.ws_row; ++i)
			std::wcout << buf[i] << '\n';
		std::wcout << L"\033[0;0H";

		char c = getchar();
		double new_x, new_y, mult = 0.2;
		switch (c) {
			case 'q': return;
			case 's': mult *= -1; // fallthrough
			case 'w':
				new_x = x+mult*cos(o);
				new_y = y+mult*sin(o);
				if (isspace(map.grid[int(new_y)][int(new_x)]))
					x = new_x, y = new_y;
				clamp(x, y);
				break;
			case 'd': mult *= -1; // fallthrough
			case 'a':
				o += mult; o = fmod(o, 2*M_PI);
				break;
			case 't': t = !t; break;
			case 'g': g = !g; break;
			case 'f': f = !f; break;
		}
	}
}

void get_ws(int sig = 0)
{
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
}

int main(int argc, char *argv[])
{
	setlocale(LC_ALL, "en_US.UTF-8");
	if (argc < 2) {
		std::wcerr << L"usage: " << argv[0] << L" <map>\n";
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
	std::wcout << L"\033[?25l";
	play_map(read_map(argv[1]));
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &oldt);
	std::wcout << L"\033[2J\033[?25h";
	return 0;
}
