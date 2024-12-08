// Day07.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <string>
#include <array>
#include <functional>
#include <vector>
#include <sstream>
#include <chrono>
#include <omp.h>

using Number = size_t;

Number plus(Number a, Number b) { return a + b; }
Number mul(Number a, Number b) { return a * b; }
Number concat(Number a, Number b) {
	Number multiplier = 1;

	while (multiplier <= b) {
		multiplier *= 10;
	}

	return b + a * multiplier;
}

using Op = Number(Number a, Number b);
std::array<std::function<Op>, 3> operators = { mul, plus, concat };

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
			Number c = op(solution, a);
			if (c <= reference)
			{
				solutions.push_back(c);
			}
		}
	}
	return solutions;
}

struct Task {
	Number testNum;
	std::vector<Number> values;
	Task(Number testNum, std::vector<Number> &&values) : testNum(testNum), values(values) {}
};

void calculateTask(Number testNum, std::vector<Number> values, Number* sums) {
	auto solutions = calculate(values.size() - 1, values, testNum);
	for (Number solution : solutions) {
		if (solution == testNum) {
			sums[omp_get_thread_num()] += testNum;
			return;
		}
	}
}

int main()
{
	auto start = std::chrono::high_resolution_clock::now();
	// Cache line
	const size_t alignment = 64;
	const size_t n = omp_get_max_threads();

	Number* sums = static_cast<Number*>(_aligned_malloc(n * sizeof(Number), alignment));
	if (!sums) {
		std::cerr << "failed to allocate aligned memory\n";
		return 1;
	}

	#pragma parallel for simd
	for (int i = 0; i < n; i++)
	{
		sums[i] = 0;
	}

	#pragma omp parallel
	{
		#pragma omp master
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

				#pragma omp task
				calculateTask(testNum, inputValues, sums);
			}
		}
	}

	Number sum = 0;
	#pragma parallel for simd reduction(+:sum)
	for (int i = 0; i < n; i++)
	{
		sum += sums[i];
	}

	_aligned_free(sums);
	std::cout << sum << std::endl;
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> duration = end - start;
	std::cout << "Execution time: " << duration.count() << " seconds" << std::endl;
	return 0;
}
