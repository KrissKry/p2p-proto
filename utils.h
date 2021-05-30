//
// Created by Michal Wiszenko on 30/05/2021.
//

#ifndef UTILS_H
#define UTILS_H

#include <mutex>
#include <iostream>
#include <vector>

void safeOutput(const std::string &str, std::mutex &m) {
    m.lock();
    std::cout << str << std::endl << std::flush;
    m.unlock();
}

char * safeInput(char *command, int max_length, std::mutex &m) {
    m.lock();
    std::cin.getline(command, max_length);
    m.unlock();
    return command;
}

std::vector<std::string> splitStringOnSpace(const std::string &s) {
    std::string temp;
    std::vector<std::string> v;
    for (char i : s) {
        if (i == ' ') {
            v.push_back(temp);
            temp = "";
        } else {
            temp.push_back(i);
        }
    }
    v.push_back(temp);
    return v;
}

#endif //UTILS_H
