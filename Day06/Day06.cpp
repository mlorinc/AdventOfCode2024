// Day06.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <algorithm>
#include <iostream>
#include <sstream>
#include <fstream>
#include <regex>
#include <vector>
#include <string>
#include <cassert>
#include <set>
#include <chrono>
#include <thread>
#include <unordered_set>
#include <Windows.h>
#include <chrono>
#include <array>


struct Vec {
	int x; int y;
	Vec(int x = 0, int y = 0) : x(x), y(y) {}
	Vec(char c) {
		switch (c)
		{
		case '^': x = 0; y = -1; break;
		case 'v': x = 0; y = 1; break;
		case '<': x = -1; y = 0; break;
		case '>': x = 1; y = 0; break;;
		default:
			throw std::invalid_argument("invalid direction character");
		}
	}

	bool operator==(const Vec& other) const {
		return x == other.x && y == other.y;
	}

	Vec rotateRight() const {
		if (x == 0 && y == -1) return Vec{ 1, 0 };
		if (x == 0 && y == 1) return Vec{ -1, 0 };
		if (x == -1 && y == 0) return Vec{ 0, -1 };
		if (x == 1 && y == 0) return Vec{ 0, 1 };
		assert(false);
	}

	char toChar() const {
		if (x == 0 && y == -1) return '^';
		if (x == 0 && y == 1) return 'v';
		if (x == -1 && y == 0) return '<';
		if (x == 1 && y == 0) return '>';
		throw std::invalid_argument("invalid vector values for direction");
	}

	uint8_t getMask() {
		if (x == 0 && y == -1) return 0x01;
		if (x == 0 && y == 1) return 0x02;
		if (x == -1 && y == 0) return 0x04;
		if (x == 1 && y == 0) return 0x08;
	}
};

struct Position {
	int x; int y;
	Position(int x = 0, int y = 0) : x(x), y(y) {}
	Position(const Position& pos) : x(pos.x), y(pos.y) {}
	Position operator+(const Vec& movement) const {
		return Position{ x + movement.x, y + movement.y };
	}

	Position operator-(const Vec& movement) const {
		return Position{ x - movement.x, y - movement.y };
	}

	Position operator-(const Position& pos) const {
		return Position{ x - pos.x, y - pos.y };
	}

	bool operator<(const Position& other) const {
		return std::tie(y, x) < std::tie(other.y, other.x);
	}

	bool operator!=(const Position& other) const {
		return x != other.x || y != other.y;
	}

	bool operator==(const Position& other) const {
		return x == other.x && y == other.y;
	}

	int distance(const Position& pos) const {
		Position diff = *this - pos;
		if (diff.x == 0) return std::abs(diff.y);
		if (diff.y == 0) return std::abs(diff.x);
		throw std::invalid_argument("invalid distance");
	}

	struct HashFunction
	{
		size_t operator()(const Position& pos) const
		{
			size_t xHash = std::hash<int>()(pos.x);
			size_t yHash = std::hash<int>()(pos.y) << 1;
			return xHash ^ yHash;
		}
	};

	struct RowLess {
		bool operator()(const Position& a, const Position& b) const {
			return a.x < b.x;
		}
	};

	struct ColumnLess {
		bool operator()(const Position& a, const Position& b) const {
			return a.y < b.y;
		}
	};
};

Position INVALID_POSITION(INT_MIN, INT_MIN);

class Map {
private:
	std::vector<std::string> map;
public:
	Map() {}

	bool isWithin(const Position& pos) const {
		return !isOutside(pos);
	}

	bool isOutside(const Position& pos) const {
		return pos.x < 0 || pos.x >= getSizeX() || pos.y < 0 || pos.y >= getSizeY();
	}

	void addObstacle(const Position& pos) {
		map[pos.y][pos.x] = '#';
	}

	void removeObstacle(const Position& pos) {
		map[pos.y][pos.x] = '.';
	}

	void addRow(std::string row) {
		map.push_back(row);
	}

	bool isBlocked(Position pos) const {
		return map[pos.y][pos.x] == '#';
	}

	int getSizeX() const { return static_cast<int>(map[0].size()); }
	int getSizeY() const { return static_cast<int>(map.size()); }

	std::string to_string(Position player, Vec movement) const {
		std::ostringstream os;
		for (int i = 0; i < getSizeY(); i++)
		{
			os << map[i] << std::endl;
		}
		return os.str();
	}
};

Map loadMap(Position &player, Vec &movement, const std::string &file = "input") {
	std::regex re(R"(\^|v|<|>)");
	std::ifstream in(file);
	std::string line;
	Map map;

	int lineCounter = 0;
	while (std::getline(in, line), !in.fail()) {
		map.addRow(line);
		std::smatch match;
		if (std::regex_search(line, match, re))
		{
			int x = static_cast<int>(match.prefix().str().size());
			movement = Vec(match.str()[0]);
			player = Position{ x, lineCounter };
		}

		lineCounter++;
	}
	return map;
}

std::unordered_set<Position, Position::HashFunction> simulateWalkVisits(const Map& map, Position startPos, Vec startMov, Position &endPos, Vec &endMov, int steps = INT_MAX) {
	Position playerPos = startPos;
	Vec movement = startMov;
	std::unordered_set<Position, Position::HashFunction> visited;
	int counter = 0;
	while (counter < steps) {
		visited.insert(playerPos);
		auto newPos = playerPos + movement;

		if (map.isOutside(newPos))
		{
			break;
		}

		if (map.isBlocked(newPos))
		{
			movement = movement.rotateRight();
		}
		else
		{	
			playerPos = playerPos + movement;
			counter++;
		}
		//SetConsoleCursorPosition(cli, coord);
		//std::cout << map.to_string(position, movement) << std::endl;
		//std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}

	endPos = playerPos;
	endMov = movement;
	return visited;
}

std::unordered_set<Position, Position::HashFunction> simulateWalkVisits(const Map& map, Position startPos, Vec startMov) {
	Position discardPos;
	Vec discardMov;
	return simulateWalkVisits(map, startPos, startMov, discardPos, discardMov);
}

bool simulateWalk(const Map& map, Position &pos, Vec &mov, std::vector<uint8_t> &directionMap, bool &cycle, int steps = INT_MAX) {
	int counter = 0;
	while (counter < steps) {
		auto newPos = pos + mov;

		if (map.isOutside(newPos))
		{
			return false;
		}

		if (map.isBlocked(newPos))
		{
			mov = mov.rotateRight();
		}
		else
		{
			pos = newPos;

			if (directionMap[pos.y * map.getSizeX() + pos.x] & mov.getMask()) {
				cycle = true;
				return true;
			}

			directionMap[pos.y * map.getSizeX() + pos.x] |= mov.getMask();
			counter++;
		}
		//SetConsoleCursorPosition(cli, coord);
		//std::cout << map.to_string(position, movement) << std::endl;
		//std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}
	return map.isWithin(pos);
}

bool isCycledWalk(const Map& map, Position startPos, Vec startMov, std::vector<uint8_t> &directionMap) {
	std::fill(directionMap.begin(), directionMap.end(), 0);	
	Position playerPos = startPos;
	Vec mov = startMov;

	bool cycle = false;
	while (!cycle) {
		if (!simulateWalk(map, playerPos, mov, directionMap, cycle)) {
			return false;
		}
	}
	return true;
}

int main()
{

	Position playerPos;
	Vec movement;
	Map map = loadMap(playerPos, movement, "input.old");
	std::vector<uint8_t> directionMap(map.getSizeX() * map.getSizeY(), 0);
	auto visited = simulateWalkVisits(map, playerPos, movement);
	auto start = std::chrono::high_resolution_clock::now();
	size_t counter = 0;
	for (auto obstacle : visited) {
		map.addObstacle(obstacle);
		auto isCycled = isCycledWalk(map, playerPos, movement, directionMap);
		map.removeObstacle(obstacle);

		if (isCycled)
		{
			counter++;
		}
	}

	std::cout << visited.size() << std::endl;
	std::cout << counter << std::endl;
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> duration = end - start;

	std::cout << "Execution time: " << duration.count() << " seconds" << std::endl;
	return 0;
}
