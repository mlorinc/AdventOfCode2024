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

const std::string xmas = "XMAS";
const std::string samx = "SAMX";

std::vector<int> getFailureVector(const std::string_view& str) {
	std::vector<int> out(str.size(), 0);
	for (int i = 2; i < str.size(); i++)
	{
		int start = i;
		int pos = out[i - 1];

		do {

			if (str[pos] == str[i - 1])
			{
				out[i] = pos + 1;
				break;
			}
			pos = out[pos];
		} while (pos > 0);
	}
	out[0] = -1;
	return out;
}

template<int SIZE>
consteval std::array<int, SIZE> getFailureVector(const char* str) {
	std::array<int, SIZE> out;
	for (int i = 2; i < SIZE; i++)
	{
		int start = i;
		int pos = out[i - 1];

		do {

			if (str[pos] == str[i - 1])
			{
				out[i] = pos + 1;
				break;
			}
			pos = out[pos];
		} while (pos > 0);
	}
	out[0] = -1;
	return out;
}

bool matches(const std::string& pattern, std::string::iterator start, std::string::iterator end, const std::vector<int>& T) {
	int pos = 0;
	for (std::string::iterator it = start; it != end && pos < pattern.size();)
	{
		if (pos == -1)
		{
			it++;
			pos = 0;
		}
		else if (pattern[pos] == *it)
		{
			pos++;
			it++;
		}
		else
		{
			pos = T[pos];
		}
	}
	return pos == pattern.size();
}

struct FSM {
	const std::vector<int>& T;
	const std::string pattern;
	int pos;

	FSM(const std::string pattern, const std::vector<int>& T) : pattern(pattern), T(T), pos(0) {}

	bool feed(char c) {
		while (true) {
			//if (xmas.find(c) == std::string::npos) return false;

			if (pos == -1)
			{
				reset();
				return false;
			}
			else if (pattern[pos] == c)
			{
				pos++;
				if (pos == pattern.size()) {
					reset();
					return true;
				}
				return false;
			}
			else
			{
				pos = T[pos];
			}
		}
	}

	void reset() {
		pos = 0;
	}
};

size_t feed(char c, FSM& fsmXmas, FSM& fsmSamx) {
	size_t counter = 0;

	if (fsmXmas.feed(c))
	{
		counter++;
	}
	if (fsmSamx.feed(c))
	{
		counter++;
	}

	return counter;
}

size_t feed(const std::string_view& str, FSM& fsmXmas, FSM& fsmSamx) {
	size_t counter = 0;
	for (auto c : str) {
		counter += feed(c, fsmXmas, fsmSamx);
	}
	return counter;
}

using FeedCharFunc = std::function<size_t(const char)>;

size_t feedDiagonal(std::vector<std::string>& matrix, int x, int y, int dx, int dy, int M, int N, FeedCharFunc feed) {
	size_t counter = 0;
	while (x < M && y < N && x >= 0 && y >= 0) {
		counter += feed(matrix[y][x]);
		x += dx;
		y += dy;
	}
	return counter;
}

int mainOld()
{
	std::ifstream in("input");
	std::string str;
	std::vector<std::string> lines;
	int lineCount = 0;
	int counter = 0;

	std::vector<int> T_XMAS = getFailureVector(xmas);
	std::vector<int> T_SAMX = getFailureVector(samx);

	FSM fsmXmas(xmas, T_XMAS), fsmSamx(samx, T_SAMX);

	auto feedString = [&fsmXmas, &fsmSamx](const std::string_view x) -> size_t { return feed(x, fsmXmas, fsmSamx); };
	auto feedChar = [&fsmXmas, &fsmSamx](const char x) -> size_t { return feed(x, fsmXmas, fsmSamx); };
	auto resetFSM = [&fsmXmas, &fsmSamx]() -> void { fsmXmas.reset(); fsmSamx.reset(); };

	std::getline(in, str);

	if (in.fail())
	{
		return 1;
	}

	int stringSize = str.size();

	// Load + search in single sentence
	while (!in.fail()) {
		resetFSM();
		lineCount++;
		counter += feedString(str);
		lines.push_back(str);
		std::getline(in, str);
	}

	// Right-Up-Down Diagonale (START)
	for (int i = 0; i < stringSize; i++)
	{
		resetFSM();
		counter += feedDiagonal(lines, i, 0, 1, 1, lineCount, stringSize, feedChar);
	}

	for (int i = 1; i < lineCount; i++)
	{
		resetFSM();
		counter += feedDiagonal(lines, 0, i, 1, 1, lineCount, stringSize, feedChar);
	}
	// Right-Up-Down Diagonale (END)

	// Left-Up-Down Diagonale (START)
	for (int i = 0; i < stringSize; i++)
	{
		resetFSM();
		counter += feedDiagonal(lines, i, 0, -1, 1, lineCount, stringSize, feedChar);
	}

	for (int i = 1; i < lineCount; i++)
	{
		resetFSM();
		counter += feedDiagonal(lines, stringSize - 1, i, -1, 1, lineCount, stringSize, feedChar);
	}
	// Left-Up-Down Diagonale (END)


	// Vertical
	for (int col = 0; col < stringSize; col++) {
		resetFSM();
		for (int row = 0; row < lineCount; row++)
		{
			counter += feedChar(lines[row][col]);
		}
	}

	//size_t offset;
	//for (const auto& line : lines) {

	//	offset = 0;
	//	while (offset + xmas.size() <= line.size()) {
	//		auto index = line.find(xmas, offset);
	//		if (index == std::string::npos)
	//		{
	//			break;
	//		}
	//		counter++;
	//		offset = index + xmas.size() + 1;
	//	}
	//}

	std::cout << counter << std::endl;
	return 0;
}
