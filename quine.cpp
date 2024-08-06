
#include <string>
#include <iostream>
std::string raw(std::string& x)
{
	return std::string{'R'}+'"'+'('+x+')'+'"';
}
std::string last = "\n\nauto& x = std::cout << str+raw(str)+';'+last;\nint main() { return 0; }\n";
std::string str = R"(
#include <string>
#include <iostream>
std::string raw(std::string& x)
{
	return std::string{'R'}+'"'+'('+x+')'+'"';
}
std::string last = "\n\nauto& x = std::cout << str+raw(str)+';'+last;\nint main() { return 0; }\n";
std::string str = )";

auto& x = std::cout << str+raw(str)+';'+last;
int main() { return 0; }
