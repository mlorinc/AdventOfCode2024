// Day08.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <chrono>

struct Vec {
	int x; int y;
	Vec(int x = 0, int y = 0) : x(x), y(y) {}
	Vec(const Vec& a, const Vec& b) {
		x = b.x - a.x;
		y = b.y - a.y;
	}

	int manhattanDistance() const {
		return std::abs(x) + std::abs(y);
	}

	Vec operator+(const Vec& vec) const {
		return Vec{ x + vec.x, y + vec.y };
	}

	Vec operator-(const Vec& vec) const {
		return Vec(vec, *this);
	}

	bool operator<(const Vec& other) const {
		return std::tie(y, x) < std::tie(other.y, other.x);
	}

	bool operator!=(const Vec& other) const {
		return x != other.x || y != other.y;
	}

	bool operator==(const Vec& other) const {
		return x == other.x && y == other.y;
	}

	bool in(int gridx, int gridy) const {
		return 0 <= x && x < gridx && 0 <= y && y < gridy;
	}

	struct HashFunction
	{
		size_t operator()(const Vec& pos) const
		{
			size_t xHash = std::hash<int>()(pos.x);
			size_t yHash = std::hash<int>()(pos.y) << 1;
			return xHash ^ yHash;
		}
	};

	struct XLess {
		bool operator()(const Vec& a, const Vec& b) const {
			return a.x < b.x;
		}
	};

	struct YLess {
		bool operator()(const Vec& a, const Vec& b) const {
			return a.y < b.y;
		}
	};
};

using VecSet = std::unordered_set<Vec, Vec::HashFunction>;
using VecList = std::vector<Vec>;

VecSet findAntinodes(const std::unordered_map<char, VecList> &antenas, int gridx, int gridy) {
	VecSet antinodes;
	for (auto frequencyIt = antenas.cbegin(), frequencyEnd = antenas.cend(); frequencyIt != frequencyEnd; frequencyIt++) {
		auto &frequencyAntenas = frequencyIt->second;
		const size_t size = frequencyAntenas.size();
		for (int i = 0; i < size - 1; i++) {
			for (int j = i + 1; j < size; j++) {
				const Vec& a = frequencyAntenas[i];
				const Vec& b = frequencyAntenas[j];

				Vec diff = b - a;
				Vec aPos = a - diff;
				Vec bPos = b + diff;

				if (aPos.in(gridx, gridy))
				{
					antinodes.insert(aPos);
				}
				if (bPos.in(gridx, gridy))
				{
					antinodes.insert(bPos);
				}
			}
		}
	}
	return antinodes;
}

int main()
{
	std::string line, prevLine;
	std::ifstream in("input");
	std::unordered_map<char, VecList> antenas;

	auto start = std::chrono::high_resolution_clock::now();
	int currentLine = 0;
	while (prevLine = std::move(line), std::getline(in, line), !in.fail()) {		
		for (int i = 0; i < line.size(); i++) {
			const char c = line[i];
			if (c != '.')
			{
				antenas[c].push_back(Vec(i, currentLine));
			}
		}
		currentLine++;
	}

	VecSet antinodes = findAntinodes(antenas, prevLine.size(), currentLine);
    std::cout << antinodes.size() << std::endl;
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> duration = end - start;
	std::cout << "Execution time: " << duration.count() << " seconds" << std::endl;
}
