// Day03.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <regex>

int main()
{
	std::ifstream in("input");
	std::regex fm(R"((mul\((\d+),(\d+)\))|(don't)|(do))");
	std::string data((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());

	auto regItBegin = std::sregex_iterator(data.begin(), data.end(), fm);
	auto regItEnd = std::sregex_iterator();
	size_t sum = 0;
	bool enabled = true;

	for (auto it = regItBegin; it != regItEnd; it++)
	{
		std::smatch matches = *it;

		if (matches[0].str().rfind("mul", 0) == 0)
		{
			if (!enabled) continue;
			auto a = std::stoi(matches[2].str());
			auto b = std::stoi(matches[3].str());
			sum += a * b;
			std::cout << a << " * " << b << std::endl;
		}
		else if (matches[0].str().rfind("don't", 0) == 0) {
			enabled = false;
		}
		else {
			enabled = true;
		}
	}
	std::cout << sum << std::endl;
}
