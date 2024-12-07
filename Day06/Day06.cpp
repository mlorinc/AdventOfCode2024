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
	int x = -1;
	std::vector<std::set<Position, Position::RowLess>> rowObstacles;
	std::vector<std::set<Position, Position::ColumnLess>> colObstacles;
public:
	Map() {}

	bool isWithin(const Position& pos) {
		return !isOutside(pos);
	}

	bool isOutside(const Position& pos) {
		return pos.x < 0 || pos.x >= x || pos.y < 0 || pos.y >= getSizeY();
	}

	void addObstacle(Position pos) {
		rowObstacles[pos.y].insert(pos);
		colObstacles[pos.x].insert(pos);
	}

	void removeObstacle(Position pos) {
		rowObstacles[pos.y].erase(pos);
		colObstacles[pos.x].erase(pos);
	}

	void addRow() {
		rowObstacles.push_back({});
	}

	bool isBlocked(Position pos) const {
		return rowObstacles[pos.y].find(pos) != rowObstacles[pos.y].end();
	}

	Position getFuturePosition(const Position& player, const Vec& movement, bool &done) const {
		done = false;
		std::set<Position>::const_iterator it;
		bool increasing = (movement.x > 0) || (movement.y > 0);
		if (movement.x != 0)
		{			
			auto& obstacles = rowObstacles[player.y];
			if (increasing) {
				it = std::upper_bound(obstacles.cbegin(), obstacles.cend(), player);
				if (it == obstacles.end()) {
					done = true;
					return Position(getSizeX() - 1, player.x);
				}
			}
			else
			{
				it = std::lower_bound(obstacles.cbegin(), obstacles.cend(), player);
				if (it == obstacles.begin()) {
					done = true;
					return Position(0, player.y);
				}
				it--;
			}
		}
		else if (movement.y != 0) {
			auto& obstacles = colObstacles[player.x];			

			if (increasing) {
				it = std::upper_bound(obstacles.cbegin(), obstacles.cend(), player);
				if (it == obstacles.end()) {
					done = true;
					return Position(player.x, getSizeY() - 1);
				}
			}
			else
			{
				it = std::lower_bound(obstacles.cbegin(), obstacles.cend(), player);
				if (it == obstacles.begin()) {
					done = true;
					return Position(player.x, 0);
				}
				it--;
			}			
		}
		else
		{
			throw std::invalid_argument("invalid movement");
		}
		
		return *it - movement;
	}

	int getSizeX() const { return x; }
	void setSizeX(int x) {
		if (this->x != x) {
			colObstacles.resize(x);
		}
		this->x = x;
	}
	int getSizeY() const { return static_cast<int>(rowObstacles.size()); }

	std::string to_string(Position player, Vec movement) const {
		std::ostringstream os;
		for (int i = 0; i < getSizeY(); i++)
		{
			std::string line(getSizeX(), '.');
			if (player.y == i) {
				line[player.x] = movement.toChar();
			}
			for (auto obstacle : rowObstacles[i]) {
				line[obstacle.x] = '#';
			}
			os << line << std::endl;
		}
		return os.str();
	}
};

Map loadMap(Position &player, Vec &movement, const std::string &file = "input") {
	std::regex re(R"(\^|v|<|>|#)");
	std::ifstream in(file);
	std::string line;
	Map map;

	int lineCounter = 0;
	while (std::getline(in, line), !in.fail()) {
		map.setSizeX(static_cast<int>(line.size()));
		map.addRow();
		int lastX = 0;
		for (std::sregex_iterator it = std::sregex_iterator(line.begin(), line.end(), re); it != std::sregex_iterator(); it++)
		{
			std::smatch match = *it;
			int x = static_cast<int>(match.prefix().str().size()) + lastX;
			lastX = x + match.size();
			if (match.str()[0] == '#')
			{
				map.addObstacle(Position{ x, lineCounter });
			}
			else
			{
				movement = Vec(match.str()[0]);
				player = Position{ x, lineCounter };
			}
		}
		lineCounter++;
	}
	return map;
}

std::unordered_set<Position, Position::HashFunction> simulateWalkVisits(const Map& map, Position startPos, Vec startMov) {
	bool done = false;
	Position playerPos = startPos;
	Vec movement = startMov;
	std::unordered_set<Position, Position::HashFunction> visited;
	visited.insert(playerPos);

	while (!done) {
		auto newPos = map.getFuturePosition(playerPos, movement, done);
		if (playerPos == newPos && !done)
		{
			movement = movement.rotateRight();
		}
		else
		{
			while (playerPos != newPos) {				
				playerPos = playerPos + movement;
				visited.insert(playerPos);
			}
		}
		//SetConsoleCursorPosition(cli, coord);
		//std::cout << map.to_string(position, movement) << std::endl;
		//std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}
	return visited;
}

bool isFiniteWalkHelper(const Map& map, Position startPos, Vec startMov, Position &prevEndPos, Position& endPos, Vec &endMov, int steps = INT_MAX) {
	bool done = false;
	Position playerPos = startPos;
	Vec movement = startMov;
	int counter = 0;
	while (!done && counter < steps) {
		auto newPos = map.getFuturePosition(playerPos, movement, done);
		if (playerPos == newPos && !done)
		{
			movement = movement.rotateRight();
		}
		else
		{
			prevEndPos = playerPos;
			playerPos = newPos;
			counter++;
		}
	}

	endPos = playerPos;
	endMov = movement;

	return done;
}

bool isCycledWalk(const Map& map, Position startPos, Vec startMov) {
	bool done = false;
	
	Position playerPosSlow = startPos, playerPosFast = startPos;
	Position playerPosSlowPrev, playerPosFastPrev;
	Vec movementSlow = startMov, movementFast = startMov;
	

	bool run = true;
	while (run) {
		if (isFiniteWalkHelper(map, playerPosFast, movementFast, playerPosFastPrev, playerPosFast, movementFast, 2)) {
			return false;
		}
		isFiniteWalkHelper(map, playerPosSlow, movementSlow, playerPosSlowPrev, playerPosSlow, movementSlow, 1);
		run = (playerPosFast != playerPosSlow || playerPosFastPrev != playerPosSlowPrev);
	}
	return true;
}

std::unordered_set<Position, Position::HashFunction> findObstacleCandidates(const Map& map, Position startPos, Vec startMov) {
	bool done = false, candidateDone = false;
	Position playerPos = startPos;
	Vec movement = startMov;
	std::unordered_set<Position, Position::HashFunction> candidates;

	while (!done) {
		auto newPos = map.getFuturePosition(playerPos, movement, done);
		if (playerPos == newPos && !done)
		{
			movement = movement.rotateRight();
		}
		else
		{
			while (playerPos != newPos) {
				if (!map.isBlocked(playerPos + movement)) {
					Vec candidateDirection = movement.rotateRight();
					Position candidateRedirectionPosition = map.getFuturePosition(playerPos, candidateDirection, candidateDone);

					if (!candidateDone)
					{
						candidates.insert(playerPos + movement);
					}
				}

				playerPos = playerPos + movement;
			}
		}
	}
	candidates.erase(startPos);
	return candidates;
}

int main()
{

	Position playerPos;
	Vec movement;
	Map map = loadMap(playerPos, movement, "input.old");
	//auto visited = simulateWalkVisits(map, playerPos, movement);
	auto start = std::chrono::high_resolution_clock::now();
	auto obstacles = findObstacleCandidates(map, playerPos, movement);
	size_t counter = 0;
	for (auto obstacle : obstacles) {
		map.addObstacle(obstacle);
		auto isCycled = isCycledWalk(map, playerPos, movement);
		map.removeObstacle(obstacle);

		if (isCycled)
		{
			counter++;
		}
	}

	//std::cout << visited.size() << std::endl;
	std::cout << obstacles.size() << std::endl;
	std::cout << counter << std::endl;
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> duration = end - start;

	std::cout << "Execution time: " << duration.count() << " seconds" << std::endl;
	return 0;
}
