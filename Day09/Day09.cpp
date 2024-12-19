// Day09.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <numeric>
#include <list>

constexpr int DOT = -1;

std::vector<int> parse(const std::vector<int> filemap) {
	std::vector<int> os;

	bool file = true;
	int id = 0;
	for (auto num : filemap)
	{
		for (int i = 0; i < num; i++)
		{
			if (file)
			{
				os.push_back(id);
			}
			else
			{
				os.push_back(DOT);
			}
		}

		if (file)
		{
			id++;
		}
		file = !file;
	}
	return os;
}

void moveLeft(const std::vector<int>& filemap, std::vector<int>& filesystem) {
	int index = filemap[0];
	int backIndex = filesystem.size() - 1;
	int freeSpaceIndex = 1;
	int freespace = filemap[freeSpaceIndex];
	int fileIndex = filemap.size() - 1;
	int fileSize = filemap[fileIndex];
	int fileId = (fileIndex / 2);
	while (index < backIndex) {
		if (freespace == 0)
		{
			freeSpaceIndex += 2;
			// Skip file
			index += filemap[freeSpaceIndex - 1];
			freespace = filemap[freeSpaceIndex];
			continue;
		}
		if (fileSize == 0)
		{
			fileIndex -= 2;
			// Skip empty space
			backIndex -= filemap[fileIndex + 1];
			fileSize = filemap[fileIndex];
			fileId--;
			continue;
		}
		filesystem[index++] = fileId;
		filesystem[backIndex--] = DOT;
		freespace--; fileSize--;
	}
}

struct Block {
	int id; int size; bool locked = false;
	Block(int id, int size) : id(id), size(size) {}
	Block(int size) : id(DOT), size(size) {}
	bool isFile() { return id != DOT; }
	void freeSpace() { id = DOT; }

	bool operator==(const Block& that) const {
		return this == &that;
	}
};

std::list<Block>::iterator mergeNeighbours(std::list<Block>& filesystem, std::list<Block>::iterator it) {
	if (it->isFile())
	{
		return it;
	}

	bool changed = false;
	if (it != filesystem.begin())
	{
		auto prev = it; prev--;
		if (prev != filesystem.begin() && !prev->isFile()) {
			it->size += prev->size;
			it = filesystem.erase(prev);
			changed = true;
		}
	}

	if (it != filesystem.end())
	{
		auto next = it; next++;
		if (next != filesystem.end() && !next->isFile()) {
			next->size += it->size;
			it = filesystem.erase(it);
			changed = true;
		}
	}

	if (changed) {
		return mergeNeighbours(filesystem, it);
	}
	else
	{
		return it;
	}
}

std::list<Block>::iterator findNextFile(std::list<Block>& filesystem, std::list<Block>::iterator it) {
	while (it != filesystem.begin()) {
		if (it->isFile())
		{
			return it;
		}
		it--;
	}
	return filesystem.begin();
}


std::list<Block>::iterator moveLeftSingleBlock(std::list<Block>& filesystem, std::list<Block>::iterator file) {
	if (file == filesystem.begin())
	{
		return file;
	}

	if (!file->isFile())
	{
		file--;
		return file;
	}

	int filesize = file->size;
	auto freeSpaceIt = filesystem.begin();

	while (freeSpaceIt != file) {
		if (freeSpaceIt->isFile())
		{
			freeSpaceIt++; 
			continue;
		}

		int freespace = freeSpaceIt->size;
		if (freespace >= filesize)
		{
			if (freespace > filesize) {
				freeSpaceIt->size = freespace - filesize;
				freeSpaceIt = filesystem.insert(freeSpaceIt, *file);
			}
			else
			{
				freeSpaceIt->id = file->id;
			}
			file->id = DOT;

			return mergeNeighbours(filesystem, file);			
		}
		else
		{
			freeSpaceIt++;
		}
	}

	file--;
	return file;
}

std::list<Block> moveLeftBlock(std::vector<int>& filemap) {
	std::list<Block> filesystem;

	for (int i = 0; i < filemap.size(); i++)
	{
		filesystem.emplace_back((i % 2 == 0) ? (i / 2) : (DOT), filemap[i]);
	}

	auto it = filesystem.end(); it--;

	while (it != filesystem.begin()) {
		it = moveLeftSingleBlock(filesystem, it);
	}

	return filesystem;
}

size_t crc(const std::vector<int>& filesystem) {
	size_t crc = 0;
	int i = 0;
	for (i = 0; i < filesystem.size(); i++)
	{
		if (filesystem[i] == DOT) continue;
		crc += i * filesystem[i];
	}
	return crc;
}

size_t crc(const std::list<Block>& filesystem) {
	size_t crc = 0;
	int i = 0;
	for (auto block : filesystem)
	{
		if (block.isFile())
		{
			for (int j = 0; j < block.size; j++) {
				crc += i * block.id;
				i++;
			}
		}
		else
		{
			i += block.size;
		}
	}
	return crc;
}

std::string to_string(const std::vector<int>& filesystem) {
	std::string out;
	std::transform(filesystem.begin(), filesystem.end(), std::back_inserter(out), [](int a) {return a != DOT ? a + '0' : '.'; });
	return out;
}

std::string to_string(const std::list<Block>& filesystem) {
	std::ostringstream os;
	for (auto block : filesystem)
	{
		if (block.isFile() && block.size > 0) {
			for (int i = 0; i < block.size; i++) {
				os << block.id;
			}
		}
		else
		{
			for (int i = 0; i < std::abs(block.size); i++) {
				os << '.';
			}
		}
	}
	return os.str();
}

int main()
{
	std::ifstream in("input");
	std::string line;
	std::vector<int> filemap;
	in >> line;
	std::transform(line.begin(), line.end(), std::back_inserter(filemap), [](const char c) { return c - '0'; });
	auto filesystem = parse(filemap);
	auto blocks = moveLeftBlock(filemap);

	std::cout << to_string(blocks) << std::endl;
	std::cout << crc(blocks) << std::endl;
}
