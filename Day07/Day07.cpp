// Day07.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <string>
#include <array>
#include <functional>
#include <vector>
#include <sstream>

using Number = size_t;

Number plus(Number a, Number b) { return a + b; }
Number mul(Number a, Number b) { return a * b; }

using Op = Number(Number a, Number b);
std::array<std::function<Op>, 2> operators = { plus, mul };

std::vector<Number> calculate(Number index, std::vector<Number> values, Number reference) {
	Number size = index;
	if (index == 0)
	{
		return { values[index] };
	}

	std::vector<Number> solutions;
	std::vector<Number> nextSolutions = calculate(index - 1, values, reference);

	Number a = values[index];
	for (auto op : operators) {
		for (Number solution : nextSolutions) {
			Number c = op(a, solution);
			if (c <= reference)
			{
				solutions.push_back(c);
			}
		}
	}
	return solutions;
}

int main()
{
	std::ifstream in("input");
	std::string testValue, line;
	Number num, testNum;
	std::vector<Number> inputValues;
	size_t sum = 0;

	while (std::getline(in, line), !in.fail()) {
		std::istringstream is(line);
		std::getline(is, testValue, ':');

		inputValues.clear();
		testNum = std::stoull(testValue);
		while (is >> num) {
			inputValues.push_back(num);
		}

		auto solutions = calculate(inputValues.size() - 1, inputValues, testNum);
		for (Number solution : solutions) {
			if (solution == testNum) {
				std::cout << line << std::endl;
				sum += testNum;
				break;
			}
		}
	}

	std::cout << sum << std::endl;
}
