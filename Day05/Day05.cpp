// Day05.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <cassert>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <map>
#include <unordered_map>
#include <vector>
#include <set>

using Rules = std::unordered_map<int, std::set<int>>;
using Rule = std::pair<int, std::set<int>>;

std::vector<int> buildHierarchy(Rules& precedenceRules, const std::set<int>& pages);

bool empty_intersection(const std::set<int>& x, const std::set<int>& y)
{
	auto i = x.begin();
	auto j = y.begin();
	while (i != x.end() && j != y.end())
	{
		if (*i == *j)
			return false;
		else if (*i < *j)
			++i;
		else
			++j;
	}
	return true;
}

int empty_intersection_debug(const std::set<int>& x, const std::set<int>& y)
{
	auto i = x.begin();
	auto j = y.begin();
	while (i != x.end() && j != y.end())
	{
		if (*i == *j)
			return *i;
		else if (*i < *j)
			++i;
		else
			++j;
	}
	return INT_MAX;
}

bool analyzePages(const std::string& line, Rules& precedenceRules, std::vector<int>& readPagesCache) {
	std::set<int> readPages;
	std::istringstream is(line);
	char garbage;
	int page;
	readPagesCache.clear();
	while (is >> page) {
		const auto& denylist = precedenceRules[page];

		if (empty_intersection(denylist, readPages)) {
			readPages.insert(page);
			readPagesCache.push_back(page);
		}
		else {
			return false;
		}
		if (!(is >> garbage)) break;
	}
	return true;
}

std::pair<int, int> analyzePages(const std::vector<int>& pages, Rules& precedenceRules) {
	std::set<int> readPages;
	for (int i = 0; i < pages.size(); i++) {
		const int page = pages[i];
		const auto& denylist = precedenceRules[page];

		auto res = empty_intersection_debug(denylist, readPages);
		if (res == INT_MAX) {
			readPages.insert(page);
		}
		else {
			return std::pair<int, int>(i, res);
		}
	}
	return std::pair<int, int>(-1, -1);
}

std::vector<int> fixOrder(Rules& precedenceRules, std::set<int>& pages) {
	return buildHierarchy(precedenceRules, pages);
}

std::vector<int> analyzeFaultyPages(const std::string& line, Rules& precedenceRules) {
	std::set<int> readPages;
	std::istringstream is(line);
	char garbage;
	int page;
	bool result = false;
	while (is >> page) {
		const auto& denylist = precedenceRules[page];
		readPages.insert(page);
		result = result || !empty_intersection(denylist, readPages);
		if (!(is >> garbage)) break;
	}

	if (result)
	{
		return fixOrder(precedenceRules, readPages);
	}

	return {};
}

int buildPrecendenceRules(std::ifstream& in, Rules& precedenceRules) {
	std::string line;
	std::set<int> allPages;
	while (std::getline(in, line), !in.fail() && !line.empty()) {
		auto index = line.find('|');
		if (index == std::string::npos)
		{
			std::cerr << "error in syntax on line: " << line << std::endl;
			return 1;
		}
		auto left = std::stoi(line.substr(0, index));
		auto right = std::stoi(line.substr(index + 1));
		allPages.insert(right);
		precedenceRules[left].insert(right);
	}

	for (auto it = precedenceRules.begin(); it != precedenceRules.end(); it++)
	{
		allPages.erase(it->first);
	}

	for (auto it = allPages.begin(); it != allPages.end(); it++)
	{
		precedenceRules[*it] = {};
	}

	if (in.fail())
	{
		std::cerr << "unexpected end of stream" << std::endl;
		return 2;
	}

	return 0;
}

std::vector<int> buildHierarchy(Rules& precedenceRules, const std::set<int>& pages) {
	std::vector<int> pageHierarchy;
	std::vector<Rule> pageHierarchyTemp;
	std::copy_if(precedenceRules.begin(), precedenceRules.end(), std::back_inserter(pageHierarchyTemp), [&](const auto& rule) {
		return pages.find(rule.first) != pages.end(); });
	std::sort(pageHierarchyTemp.begin(), pageHierarchyTemp.end(), [=](const Rule& a, const Rule& b) -> bool {
		return a.second.find(b.first) != a.second.end();
		});

	// Ensure pageHierarchy is sized correctly before transforming
	pageHierarchy.resize(pageHierarchyTemp.size());

	std::transform(pageHierarchyTemp.cbegin(), pageHierarchyTemp.cend(), pageHierarchy.begin(), [=](const Rule& a) -> int {return a.first; });
	return pageHierarchy;
}

int main()
{
	std::ifstream in("input");
	std::string line;
	Rules precedenceRules;
	int status = buildPrecendenceRules(in, precedenceRules);

	if (status != 0)
	{
		return status;
	}

	size_t sum = 0;
	while (std::getline(in, line), !in.fail()) {
		auto pages = analyzeFaultyPages(line, precedenceRules);
		if (!pages.empty())
		{
			sum += pages[pages.size() / 2];
			std::cout << line << " | ";
			std::copy(pages.cbegin(), pages.cend(), std::ostream_iterator<int>(std::cout, ","));
			std::cout << std::endl;
			auto result = analyzePages(pages, precedenceRules);

			if (result.first != -1)
			{
				std::cerr << "error at " << result.first << " in " << pages[result.first] << " after " << result.second << std::endl;
				return 3;
			}
		}
	}

	std::cout << sum << std::endl;

	return 0;
}
