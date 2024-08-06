/* Small snow generator */
/* Written for Christmas Eve 2021, when I still sucked ;-) */
#pragma GCC optimize("Ofast")
#include <bits/stdc++.h>
#if defined(_WIN32)
	#include <Windows.h>
	#include <conio.h>
#elif defined(__unix__)
	#include <sys/ioctl.h>
#endif

static inline int
usage(char *argv[])
{
	std::cerr << "usage: " << argv[0] << " [-g|-cX]";
	exit(EXIT_FAILURE);
}

static inline void
getsiz(int& W, int& H, int col)
{
	#if defined(_WIN32)
		/* Get terminal size and set colours */
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
		W = (int)(csbi.srWindow.Right - csbi.srWindow.Left + 1);
		H = (int)(csbi.srWindow.Bottom - csbi.srWindow.Top + 1);

		HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(console, col);

	#elif defined(__unix__)
		struct winsize w;
		ioctl(fileno(stdout), TIOCGWINSZ, &w);
		W = (int)(w.ws_col);
		H = (int)(w.ws_row);
		if (col)
			std::system(("tput setab " + std::to_string(col)).c_str());
	#endif
}

static inline void
cls(void)
{
	#if defined (__unix__)
		std::cout << "\x1B[2J\x1B[H";
	#elif defined(__WIN32__)
		HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleCursorPosition(console, (COORD){ 0, 0 });
	#endif
}

int
main(int argc, char *argv[])
{
jmp:	
	bool msg = false;
	int col, i, j;

	/** Parsing args **/
	#if defined(_WIN32)
		col = 15;
	#elif defined(__unix__)
		col = 0;
	#endif
	if (argc > 1) {
		if (!strcmp(argv[1], "--help"))
			usage(argv);
		else if (!strcmp(argv[1], "-g"))
			msg = true;
		else if (argv[1][0] == '-' && argv[1][1] == 'c')
      col = std::atoi(argv[1]+2);
		else
			usage(argv);
	}

	/** Generate snow pattern **/
	cls();
	std::vector<std::string> x(1024);
	int H = 0, W = 0, k = 0;

	getsiz(W, H, col);
	if (!H || !W) H = W = 1024;

	srand(time(NULL));
	for (i = 0; i < H; ++i) {
		for (j = 0; j < (W >> 1); ++j) {
			if ((int)rand() % 2) {
				x[i] += ((int)rand() % 2) ? '*' : '+';
				x[i] += ' ';
			} else {
				x[i] += "  ";
			}
		}
		if (W >> 1)
			x[i].pop_back();
	}

	/** -g flag **/
	auto wait = [](int ms) {
		std::this_thread::sleep_for(std::chrono::milliseconds(ms));
	};
	if (msg) {
		for (i = 0; i < H-2; ++i) {
			std::cout << x[i] << '\n';
			wait(35);
		}
		std::cout << "Keep coding! ;-)";
		std::exit(EXIT_SUCCESS);
	}

	/** No -g flag **/
	for (;;) {
		int hb = H, wb = W;
		getsiz(W, H, col);
		if (W != wb || H != hb)
			goto jmp;

		std::string y = x[H-1];
		std::rotate(x.rbegin(), x.rbegin() + 1, x.rend());
		for (x[0] = y, i = 0; i < H-1; ++i)
			std::cout << x[i] << '\n';
		wait(45);
		cls();
	}
}
