// Day10.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <queue>
#include <tuple>
#include <unordered_set>
#include <unordered_map>

constexpr int inf = INT_MAX;
constexpr int deadend = -1;
constexpr int invalid = -1;

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

Vec invalidVector(-1, -1);

using Grid = std::vector<std::vector<int>>;
using MemoryGrid = std::vector<std::vector<Vec>>;
using EndVisits = std::unordered_map<Vec, std::unordered_set<Vec, Vec::HashFunction>, Vec::HashFunction>;
using EndVisitsSecondPart = std::unordered_map<Vec, int, Vec::HashFunction>;

void pushValid(std::vector<Vec> &neighbors, const Grid& grid, int level, Vec pos) {
	if (!(0 <= pos.x && pos.x < grid[0].size() && 0 <= pos.y && pos.y < grid.size()))
	{
		return;
	}

	auto data = grid[pos.y][pos.x];
	if (data != invalid && data == level + 1)
	{
		neighbors.push_back(pos);
	}
}

std::vector<Vec> getNeighbors(const Grid &grid, Vec pos) {
	std::vector<Vec> neighbors;
	int level = grid[pos.y][pos.x];
	pushValid(neighbors, grid, level, pos - Vec(1, 0));
	pushValid(neighbors, grid, level, pos + Vec(1, 0));
	pushValid(neighbors, grid, level, pos - Vec(0, 1));
	pushValid(neighbors, grid, level, pos + Vec(0, 1));
	return neighbors;
}

void djikstraFirstPart(const Grid& grid, EndVisits &visits, Vec start) {
	std::queue<Vec> q;
	q.push(start);

	while (!q.empty()) {
		auto pos = q.front();
		q.pop();

		if (grid[pos.y][pos.x] == 9)
		{
			visits[start].insert(pos);
		}
		else {
			auto neighbours = getNeighbors(grid, pos);
			for (auto& n : neighbours) {
				q.push(n);
			}
		}
	}
}

void djikstraSecondPart(const Grid& grid, EndVisitsSecondPart& visits, Vec start) {
	std::queue<Vec> q;
	q.push(start);

	while (!q.empty()) {
		auto pos = q.front();
		q.pop();

		if (grid[pos.y][pos.x] == 9)
		{
			visits[start]++;
		}
		else {
			auto neighbours = getNeighbors(grid, pos);
			for (auto& n : neighbours) {
				q.push(n);
			}
		}
	}
}

int main()
{
    std::ifstream in("input");
    std::string line;
	Grid grid;
	std::vector<Vec> starts;
	EndVisits visits;
	EndVisitsSecondPart visitsSecond;

	int lineNum = 0;
	while (in >> line) {
		std::vector<int> l(line.size(), invalid);
		for (int i = 0; i < line.size(); i++) {
			if (line[i] != '.')
			{
				l[i] = line[i] - '0';
			}

			if (l[i] == 0)
			{
				starts.push_back(Vec(i, lineNum));				
				visits[Vec(i, lineNum)] = {};
				visitsSecond[Vec(i, lineNum)] = 0;
			}
		}
		grid.push_back(std::move(l));
		lineNum++;
	}

	for (Vec start : starts) {
		//djikstraFirstPart(grid, visits, start);
		djikstraSecondPart(grid, visitsSecond, start);
	}

	size_t sum = 0;
	//for (auto visit : visits) {
	//	sum += visit.second.size();
	//}

	for (auto visit : visitsSecond) {
		sum += visit.second;
	}

	std::cout << sum << std::endl;
	return 0;
}
