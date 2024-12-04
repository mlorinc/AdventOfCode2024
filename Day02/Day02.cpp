// Day02.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>

bool static inline isSafe(int a, int b) {
    int diff = std::abs(a - b);
    return 1 <= diff && diff <= 3;
}

bool static inline isSafe(int a, int b, bool increasing) {
    return (increasing ? a < b : a > b) && isSafe(a, b);
}

bool static inline isTrendIssue(int a, int b, bool increasing) {
    return !(increasing ? a < b : a > b);
}

bool static isSafeNoDampener(const std::string &line) {
    std::stringstream builder;
    int level, lastLevel;
    builder << line;
    builder >> lastLevel;
    builder >> level;

    if (!isSafe(lastLevel, level))
    {
        return false;
    }

    bool increasing = level > lastLevel;
    lastLevel = level;
    while (builder >> level) {
        if (!isSafe(lastLevel, level, increasing))
        {
            return false;            
        }
        lastLevel = level;
    }

    return true;
}

bool static isSafeDampener(const std::string& line) {
    std::stringstream builder; builder << line;
    std::vector<int> levels;
    int tmp, up = 0, down = 0;
    int a = -1, b = -1;
    while (builder >> tmp) {
        if (!levels.empty() && levels.back() < tmp) {
            up++;
        }
        levels.push_back(tmp);
    };
    down = levels.size() - up - 1;
    bool increasing = up >= down;

    if (std::min(up, down) > 1) {
        return false;
    }

    for (int i = 1; i < levels.size(); i++)
    {
        if (!isSafe(levels[i-1], levels[i], increasing))
        {
            a = i - 1;
            b = i;
            break;
        }
    }

    bool result = true;

    for (int i = 0; i < levels.size() - 1; i++)
    {
        if (i == a)
        {
            if (i == 0)
            {
                continue;
            }

            if (!isSafe(levels[i-1], levels[i + 1], increasing)) {
                result = false;
                break;
            }
        }
        else if (!isSafe(levels[i], levels[i+1], increasing))
        {
            result = false;
            break;
        }
    }

    if (result) return true;

    for (int i = 0, j = 1; j < levels.size();)
    {
        if (j == b)
        {
            j++;
            if (j == levels.size()) return true;
        }

        if (!isSafe(levels[i], levels[j], increasing))
        {
            return false;            
        }

        i = j;
        j++;
    }

    return true;
}

int main()
{
    std::ifstream in("input");
    std::string line;
    int safeCount = 0;
    while (std::getline(in, line) && !in.fail()) {
        if (isSafeDampener(line))
        {
            safeCount++;
        }
    }
    std::cout << safeCount << std::endl;
}
