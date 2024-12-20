// Day11.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <cmath>
#include <list>
#include <fstream>
#include <string>
#include <unordered_map>

void applyRuleOne(std::unordered_map<size_t, size_t> &stones) {
    stones[1] += stones[0];
    stones[0] = 0;
}

std::pair<size_t, size_t> applyRuleTwo(size_t stone, bool &applied) {
    applied = false;
    size_t digits = std::log10(stone) + 1;

    if (digits % 2 != 0)
    {
        applied = false;
        return {0, 0};
    }

    size_t half = digits / 2;
    size_t div = std::pow(10, half);
    size_t a = stone / div;
    size_t b = stone % div;
    applied = true;
    return { a, b };
}

size_t applyRuleThree(size_t stone) {
    return stone * 2024;
}

int main()
{
    std::ifstream in("input");
    size_t stone;
    std::unordered_map<size_t, size_t> stones;
    std::vector<std::pair<size_t, size_t>> pairs;

    while (in >> stone) stones[stone]++;

    for (size_t i = 0; i < 75; i++)
    {
        for (auto it = stones.begin(); it != stones.end(); it++) {
            if (it->first == 0)
            {
                continue;
            }

            bool applied = false;
            size_t count = it->second;
            auto pair = applyRuleTwo(it->first, applied);
            
            if (applied) {
                pairs.emplace_back(pair.first, count);
                pairs.emplace_back(pair.second, count);             
            }
            else {
                size_t newVal = applyRuleThree(it->first);
                pairs.emplace_back(newVal, count);
            }
            stones[it->first] = 0;
        }

        applyRuleOne(stones);
        for (auto pair : pairs) {
            stones[pair.first] += pair.second;
        }
        pairs.clear();
    }

    //std::copy(stones.begin(), stones.end(), std::ostream_iterator<size_t>(std::cout, ", "));
    //std::cout << stones.size() << std::endl;

    size_t sum = 0;
    for (auto it = stones.begin(); it != stones.end(); it++) {
        sum += it->second;
    }

    std::cout << sum << std::endl;
    return 0;
}
