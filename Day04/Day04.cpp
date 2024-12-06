// Day04.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ostream>
#include <algorithm>
#include <iterator>
#include <sstream>
#include <array>
#include <functional>

std::string expectedResult = "MAS";

struct Candidate {
	std::stringstream a, b;
	const int aPosition, maxSize;

	Candidate(std::string_view prevLine, int aPosition) : aPosition(aPosition), maxSize(prevLine.size()) {
		if (aPosition > 0) a << prevLine[aPosition - 1];
		if (aPosition + 1 < maxSize) b << prevLine[aPosition + 1];
		a << "A";
		b << "A";
	}

	bool valid() { return 0 < aPosition && aPosition + 1 < maxSize; }

	bool finish(std::string_view line) {
		a << line[aPosition + 1];
		b << line[aPosition - 1];
		std::string aStr = a.str(), bStr = b.str();
		std::sort(aStr.begin(), aStr.end());
		std::sort(bStr.begin(), bStr.end());
		return aStr == expectedResult && bStr == expectedResult;
	}
};

int main()
{
	std::ifstream in("input");
	std::string line, previousLine;
	std::vector<Candidate> solutions;
	std::sort(expectedResult.begin(), expectedResult.end());
	size_t counter = 0;

	while (std::getline(in, line), !in.fail()) {
		if (previousLine.empty())
		{
			previousLine = line;
			continue;
		}

		for (auto &solution : solutions)
		{
			if (solution.finish(line)) counter++;
		}
		solutions.clear();

		for (int i = 0; i < line.size(); i++)
		{
			if (line[i] == 'A') {
				Candidate x(previousLine, i);
				if (x.valid()) solutions.push_back(std::move(x));
			}
		}
		previousLine = line;
	}

	std::cout << counter << std::endl;
	return 0;
}
