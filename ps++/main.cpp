#include <codecvt>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <istream>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

class outstream_t
{
private:
	std::wofstream out;
	bool file;
public:
	outstream_t() : file(false) {}
	~outstream_t()=default;

	template<typename T>
	std::wostream& operator<<(T const& t)
	{
		return (file ? out : std::wcout) << t;
	}

	void open(std::string const& filename)
	{
		out.open(filename);
		this->file = true;
	}
} outstream;

static inline void eval(std::wistream& in)
{
	std::wstring line;
	wchar_t quote = 0;

	outstream << "#include <iostream>\n";
	outstream << "int q, w, e, r, t, y, u, i, o, p,\n"
	           "    a, s, d, f, g, h, j, k, l,\n"
	           "    z, x, c, v, b, n, m;\n";
	outstream << "int nr, ok, max, min; /* Common variable names */\n";
	outstream << "int main()\n";
	outstream << "{\n";

	in.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));
	while (std::getline(in, line)) {
		if (line[0] == '#' && line[1] == '!')
			continue;

		line += L'\n';

		std::wstring str, out, comma_mode = L",", identf_for_stmt;
		bool finish_do_while = false;
		bool finish_semicoln = true;
		bool finish_for_stmt = false;
		bool output_newline = false;

		for (auto c = line.begin(); c != line.end(); ++c) {
			if (!quote) {
				switch (*c) {
					case L'\'': /* FALLTHROUGH */
					case L'"':
						quote = *c;
					default:
						str += *c;
						break;

					case L'|': /* FALLTHROUGH */
					case L'│':
						out += L'\t';
						break;
					case '[':  /* FALLTHROUGH */
					case ']':
						break;
					case L'┌': /* FALLTHROUGH */
					case L'■':
						finish_semicoln = false;
						break;
					case L'└':
						out += L"}";
						break;
					
					case L'': /* FALLTHROUGH */
					case L'←':
						if (finish_for_stmt)
							identf_for_stmt = str;
						str += L"=";
						break;
					
					case L',':
						if (finish_for_stmt)
							str += L";" + identf_for_stmt+L"<=";
						else {
							output_newline = false;
							str += comma_mode;
						}
						break;
					case L'=': str += L"=="; break;
					case L'≠': str += L"!="; break;
					case L'≥': str += L">="; break;
					case L'≤': str += L"<="; break;
					case L'\t': /* FALLTHROUGH */
					case L'\v': /* FALLTHROUGH */
					case L'\r': /* FALLTHROUGH */
					case L'\n': /* FALLTHROUGH */
					case L'\f': /* FALLTHROUGH */
					case L' ':
						if (!str.empty()) {
							if (str == L"citește" || str == L"citeşte") {
								str = L"std::cin >>";
								comma_mode = L" >> ";
							}
							if (str == L"scrie") {
								str = L"std::cout <<";
								comma_mode = L" << ";
								output_newline = true;
							}
							if (str == L"pentru") {
								str = L"for (";
								finish_for_stmt = true;
							}
							if (str == L"dacă")
								str = L"if (";
							if (str == L"altfel") {
								finish_semicoln = false;
								out.erase(0, 1);
								str = L"} else {";
							}
							if (str == L"cât")
								str = L"while (";
							if (str == L"atunci")
								str = L") {";
							if (str == L"execută") {
								str = L") {";
								if (finish_for_stmt)
									str = L";" + identf_for_stmt + L"++" + str;
							}
							if (str == L"repetă")
								str = L"do {";
							if (str == L"până") {
								str = L" while (!(";
								finish_do_while = true;
							}
							if (str == L"și" || str == L"şi")
								str = L"&&";
							if (str == L"sau")
								str = L"||";
							if (str == L"când" || str == L"timp")
								str.clear();

							out += str;
							if (*c != L'\n')
								out += L' ';
							str.clear();
						}
						break;
				}
			} else {
				str += *c;
				if (*c == quote)
					quote = 0;
				if (*c == L'\\')
					str += *++c;
			}
		}
		if (output_newline)
			out += L"<< std::endl";
		if (finish_do_while)
			out += L"))";
		if (finish_semicoln)
			out += L";";

		outstream << '\t' << out << '\n';
	}
	outstream << "}" << std::endl;
}

static inline void usage(char *progname)
{
	std::cerr << "usage: " << progname << " [-h|-x|-o <out>][file|-...]" << std::endl;
	std::exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
	bool xflag = false;
	int opt;
	while ((opt = getopt(argc, argv, "hxo:")) != -1) {
		switch (opt) {
			case 'o':
				outstream.open(optarg);
				break;
			case 'x':
				xflag = true;
				break;
			case 'h': /* FALLTHROUGH */
			case '?':
				usage(*argv);
				break;
		}
	}
	if (optind >= argc)
		eval(std::wcin);
	for (; optind < argc; ++optind) {
		char tmpfil[] = "/tmp/ps-XXXXXX";
		if (xflag) {
			int ret = mkstemp(tmpfil);
			close(ret);
			outstream.open((std::string)tmpfil+".cpp");
		}

		if (!std::strcmp(argv[optind], "-")) {
			eval(std::wcin);
			continue;
		}

		std::wifstream f;
		f.open(argv[optind]);
		if (f.fail()) {
			perror(argv[optind]);
			std::exit(EXIT_FAILURE);
		} else {
			eval(f);
		}
		if (xflag) {
			static auto exec_cmd = [](char *argv[])
			{
				pid_t pid = fork();
				if (pid == 0) {
					execvp(*argv, argv);
					_exit(0);
				} else
					wait(NULL);
			};
			auto compiler = getenv("CXX");
			if (!compiler)
				compiler = (char*)"g++";
			exec_cmd((char*[]) { compiler, ((std::string)tmpfil+".cpp").data(), (char*)"-o", tmpfil, NULL });
			exec_cmd((char*[]) { tmpfil, NULL });
		}
	}
	return EXIT_SUCCESS;
}
