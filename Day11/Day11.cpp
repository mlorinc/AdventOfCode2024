// Day11.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <cmath>
#include <list>
#include <fstream>
#include <string>

std::list<size_t>::iterator applyRuleOne(std::list<size_t>& list, std::list<size_t>::iterator stone) {
    if (*stone == 0)
    {
        *stone = 1;
        stone++;
        return stone;
    }
    return stone;
}

std::list<size_t>::iterator applyRuleTwo(std::list<size_t> &list, std::list<size_t>::iterator stone) {
    size_t digits = std::log10(*stone) + 1;

    if (digits % 2 != 0)
    {
        return stone;
    }

    size_t half = digits / 2;
    size_t div = std::pow(10, half);
    size_t a = *stone / div;
    size_t b = *stone % div;

    *stone = a;
    stone++;
    auto bIt = list.insert(stone, b); 
    bIt++;
    return bIt;
}

std::list<size_t>::iterator applyRuleThree(std::list<size_t>& list, std::list<size_t>::iterator stone) {
    *stone = *stone * 2024;
    stone++;
    return stone;
}

int main()
{
    std::ifstream in("input");
    std::list<size_t> stones;
    size_t stone;

    while (in >> stone) stones.push_back(stone);

    for (size_t i = 0; i < 75; i++)
    {
        for (auto it = stones.begin(); it != stones.end();) {
            auto tempIt = it;            
            if ((it = applyRuleOne(stones, it)) != tempIt) {
                continue;
            }
            if ((it = applyRuleTwo(stones, it)) != tempIt) {
                continue;
            }
            if ((it = applyRuleThree(stones, it)) != tempIt) {
                continue;
            }
        }
    }

    //std::copy(stones.begin(), stones.end(), std::ostream_iterator<size_t>(std::cout, ", "));
    std::cout << stones.size() << std::endl;
    return 0;
}
