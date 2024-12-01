// Day01.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <set>
#include <map>

static int difference(std::multiset<int> leftSet, std::multiset<int> rightSet)
{
    int sum = 0;
    for (auto leftIt = leftSet.cbegin(), rightIt = rightSet.cbegin(); leftIt != leftSet.cend(); leftIt++, rightIt++)
    {
        sum += std::abs(*leftIt - *rightIt);
    }
    return sum;
}

static int similarity(std::multiset<int> leftSet, std::multiset<int> rightSet)
{
    std::map<int, int> counts;
    int sum = 0;
    for (const auto num : leftSet) {
        if (counts.find(num) == counts.end())
        {
            counts[num] = rightSet.count(num);
        }
        sum += num * counts[num];
    }

    return sum;
}

int main()
{
    std::ifstream in("input");
    std::multiset<int> leftSet, rightSet;

    int left, right;
    while (in >> left >> right) {
        leftSet.insert(left);
        rightSet.insert(right);
    }

    std::cout << difference(leftSet, rightSet) << std::endl;
    std::cout << similarity(leftSet, rightSet) << std::endl;
}
